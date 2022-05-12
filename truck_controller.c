/**
 * File: truck_controller.c
 * Description: Genetic self driven truck controller.
 *              This is the controller for the truck,
 *              it uses the Webots API and the evlearn.h
 *              header wich implements a genetic algorithm
 *              to find suitable values in a weight matrix
 *              used as chromosome for a given optimisation
 *              function.
 *
 * Author: Shepherdsoft_s
 * Contact: shepherdsoft@outlook.com
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

#include <webots/supervisor.h>
#include <webots/robot.h>
#include <webots/lidar.h>
#include <webots/vehicle/driver.h>

#include "evlearn.h" //this is the header of the genetic algorithm itself

#define TIME_STEP 64

#define LIDAR_PRECISION 9 //must be the same as in the simulator, set lidar sensors precision
                          //it will be the number of rays of the lidar
#define MAX_RANGE 12   //in meters
#define MIN_RANGE 1.3  //
#define MAX_RPM 2250
#define MIN_RPM 700
#define MAX_SPEED 200 //in km/h

int p_index; //index of the individual in the population
int step; //the step determines the genetic operator performed in the main loop
int generation;
double time;
double measure; //speed measured each iteration of the main loop
double m_count; //measure count, it is used to fin the medium value
const double *translation;
const double *rotation;

/**
 * Webot references and devicetags to handle nodes
 */
WbDeviceTag lidar; // the precision of lidars in the simulator must be the same as LIDAR_PRECISION
WbDeviceTag lidar_left;
WbDeviceTag lidar_right;
WbNodeRef truck;

WbFieldRef translation_f;
WbFieldRef rotation_f;

/**
 * This 3 declarations are mandatory to declare this two variables,
 * they are used by the header evlearn.h implementation
 * as global variables. The min max matrix contains the maximum
 * and minimum for each value in the chromosome.
 */
Individual population[POPULATION_SIZE];
FILE *file;
const double min_max[CHROM_ARRAY_SIZE*2][CHROM_SIZE] = {{0,0,0,0,-1,-1,-1,-1,-1},       //TO CHANGE
                                                        {1,1,1,1,1,0,0,0,0},
                                                        {0,0,0,0,0,0,0,0,0},
                                                        {.1,.1,.1,.1,.1,.1,.1,.1,.1},
                                                        {-.1,-.1,-.1,-.1,-.1,-.1,-.1,-.1,-.1},
                                                        {0,0,0,0,0,0,0,0,0},
                                                        {0,0,0,0,0,0,0,0,0},
                                                        {0,0,0,-1,-1,-1,0,0,0},
                                                        {0,0,0,0,0,0,0,0,0},
                                                        {1,1,1,1,1,1,1,1,1},
                                                        {0.5,0,0,0,0,0,0,0,0},
                                                        {1,0,0,0,0,0,0,0,0}};

/**
 * Check if the value given is out of the {min, max}
 * interval.
 *
 * @brief truncate_value
 * @param value the value to check
 * @param min minimum returned
 * @param max maximum returned
 * @return a value within the interval
 */
double truncate_value(double value, double min, double max)
{
    if (value > max) {
        return max;
    }
    if (value < min) {
        return min;
    }
    return value;
}

/**
 * This is used to determine if there is a collision occuring,
 * also it checks if the truck stops and simply consider it
 * as a collision.
 *
 * @brief collisioning
 * @param l_num number of lidar sensors
 * @param travelled_time
 * @param sensor_readings a vector with every lidar readding (wich is another vector)
 * @return a boolean value determining if there is a collision
 */
bool collisioning(int l_num, double travelled_time, const float *sensor_readings[l_num])
{
    for (int j=0; j<l_num; j++) {
        for (int k=0; k<LIDAR_PRECISION; k++) {
            if (j==0) {
                if (*(sensor_readings[j] + k) <= MIN_RANGE) {
                    return true;
                }
            }
            else {
                if (*(sensor_readings[j] + k) <= MIN_RANGE - 1) {
                    return true;
                }
            }
        }
    }

    //if the velocity is too slow so the truck has stopped
    //return true aswell to prevent the truck to stay stopped
    if ((wbu_driver_get_current_speed() < 2 &&
        travelled_time > 5) ||
        travelled_time >= 600) {
        return true;
    }
    return false;
}

/**
 * The effective evaluation function has to be implemented so it can
 * be pointed for the evaluate_population of the evlearn.h header
 * implementation, this is done this way because the header can be
 * used whatever the specific application is.
 *
 * @brief effective_evaluation
 * @param individual the individual index each recursive call,
 *                   should be initially 0.
 */
void effective_evaluation(int individual)
{
    bool collision = false;
    int gear = wbu_driver_get_gear();
    double throttle = 1;
    double brake = 0;
    double steering = 0;
    double gear_coef = 0; //this determines when to change the gear
    double rpm = wbu_driver_get_rpm();
    double speed = wbu_driver_get_current_speed();
    double avg_velocity = 0;
    double travelled_time = wb_robot_get_time() - time;
    double reading[LIDAR_PRECISION];
    double reading_left[LIDAR_PRECISION];
    double reading_right[LIDAR_PRECISION];
    const float *lidar_reading = wb_lidar_get_range_image(lidar);
    const float *lidar_reading_left = wb_lidar_get_range_image(lidar_left);
    const float *lidar_reading_right = wb_lidar_get_range_image(lidar_right);

    const float *lidars[3] = {lidar_reading, lidar_reading_left, lidar_reading_right};

    //read the sensors and store normalized data in "readings" arrays to be
    //able to change values for normalization as wb_lidar_get_range_image()
    //has a constant return value
    for (int i=0; i<LIDAR_PRECISION; i++) {
        reading[i] = 1 - (*(lidar_reading + i)/MAX_RANGE);
        reading_left[i] = 1 - (*(lidar_reading_left + i)/MAX_RANGE);
        reading_right[i] = 1 - (*(lidar_reading_right + i)/MAX_RANGE);

        //this check is to prevent the readings to be infinite, NAN or
        //out of range
        if(*(lidar_reading + i) == NAN ||
           *(lidar_reading + i) > MAX_RANGE ||
           *(lidar_reading + i) < MIN_RANGE) {
           reading[i] = 0;
        }
        if(*(lidar_reading_left + i) == NAN ||
           *(lidar_reading_left + i) > MAX_RANGE ||
           *(lidar_reading_left + i) < MIN_RANGE) {
           reading_left[i] = 0;
        }
        if(*(lidar_reading_right + i) == NAN ||
           *(lidar_reading_right + i) > MAX_RANGE ||
           *(lidar_reading_right + i) < MIN_RANGE) {
           reading_right[i] = 0;
        }

        //rays in the extremes of the lateral lidars can interfere with
        //the frontal one thus they are ignored
        reading_left[0] = 0;
        reading_left[1] = 0;
        reading_left[7] = 0;
        reading_left[8] = 0;

        reading_right[0] = 0;
        reading_right[1] = 0;
        reading_right[7] = 0;
        reading_right[8] = 0;
    }

    for (int i=0; i<CHROM_SIZE; i++) {
        steering += population[individual].chromosome[0][i] * reading[i];
        steering += population[individual].chromosome[1][i] * reading_left[i];
        steering += population[individual].chromosome[2][i] * reading_right[i];
        throttle += population[individual].chromosome[3][i] * reading[i];
        brake += population[individual].chromosome[4][i] * reading[i];
    }
    gear_coef = population[individual].chromosome[5][0];

    steering = truncate_value(steering, -.5, .5);
    throttle = truncate_value(throttle, .2, 1);
    if (speed > 250 || speed < 0) {
        speed = 0;
    }
    if (speed >= MAX_SPEED) {
        throttle = 0.22;
    }

    wbu_driver_set_throttle(throttle);
    wbu_driver_set_steering_angle(steering);
    if (throttle < 0.21) {
        brake = brake * (speed/MAX_SPEED);
        brake = truncate_value(brake, 0, 1);
        wbu_driver_set_brake_intensity(brake);
    }
    if(rpm/(MAX_RPM-MIN_RPM) > gear_coef && gear < wbu_driver_get_gear_number()) {
        gear++;
        wbu_driver_set_gear(gear);
    }
    if(rpm/(MAX_RPM-MIN_RPM) < 1-gear_coef && gear > 2) {
        gear--;
        wbu_driver_set_gear(gear);
    }

    //printing the truck parametters
    printf("throttle: %.2f\n", wbu_driver_get_throttle());
    printf("brake: %.2f\n", wbu_driver_get_brake_intensity());
    printf("steering: %.2f\n", wbu_driver_get_steering_angle());
    printf("gear: %d\n", wbu_driver_get_gear());
    printf("rpm: %.2f\n", wbu_driver_get_rpm());
    printf("speed: %.2f\n", wbu_driver_get_current_speed());
    printf("individual: %d\n", p_index);
    printf("generation: %d\n", generation);
    fflush(stdout);

    //average velocity each main loop iteration
    measure += speed*0.2778;
    if (m_count > 0) {
        avg_velocity = measure/m_count;
    }
    m_count++;

    collision = collisioning(3, travelled_time, lidars);

    if (collision) {
        population[individual].fitness = travelled_time*avg_velocity;
        time = wb_robot_get_time();
        m_count = 1;
        measure = 0;

         if (individual == POPULATION_SIZE -1) {
            generation++;
            p_index = 0;
            step = 1;
        }
        else {
            p_index++;
        }
        wb_supervisor_field_set_sf_vec3f(translation_f, translation);
        wb_supervisor_field_set_sf_rotation(rotation_f, rotation);
        wbu_driver_set_throttle(0);
        wbu_driver_set_brake_intensity(0);
        wbu_driver_set_gear(2);
    }
}

/*
 * This is the main program.
 * The arguments of the main function can be specified by the
 * "controllerArgs" field of the Robot node
 */
int main(int argc, char **argv)
{
    /* necessary to initialize webots*/
    wb_robot_init();
    wbu_driver_init();

    srand(getpid());

    p_index = 0; //index of the individual in the population
    step = 0; //the step determines the genetic operator performed in the main loop
    generation = 0;
    time = 0;
    measure = 0; //speed measured each iteration of the main loop
    m_count = 1; //measure count, it is used to fin the medium value

    truck = wb_supervisor_node_get_from_def("WEBOTS_VEHICLE0");
    translation_f = wb_supervisor_node_get_field(truck, "translation");
    rotation_f = wb_supervisor_node_get_field(truck, "rotation");

    translation = wb_supervisor_field_get_sf_vec3f(translation_f);
    rotation = wb_supervisor_field_get_sf_rotation(rotation_f);
    lidar = wb_robot_get_device("Sick LMS 291");
    lidar_left = wb_robot_get_device("Sick LMS 291 left");
    lidar_right = wb_robot_get_device("Sick LMS 291 right");

    wb_lidar_enable(lidar, TIME_STEP);
    wb_lidar_enable(lidar_left, TIME_STEP);
    wb_lidar_enable(lidar_right, TIME_STEP);
    wb_lidar_enable_point_cloud(lidar);
    wb_lidar_enable_point_cloud(lidar_left);
    wb_lidar_enable_point_cloud(lidar_right);

    /*necessary to initialize evlearn*/ 
    initialize_population(0);

    //set initial gear to 2 so that the truck can start moving
    wbu_driver_set_gear(2);

    //Main loop//////////////////////////////////////////////////////
    while (wb_robot_step(TIME_STEP) != -1) {
        wbu_driver_step();
        //this switch statement is basically a state machine
        //to apply the genetic operators
        switch (step) {
            case 0:
                evaluate_population(p_index, effective_evaluation);
                //the conditions to change to step 1 are inside the
                //ef function
            break;

            case 1:
                write_file(0);
                select_population(0, 6);
                step = 2;
            break;

            case 2:
                cross_population();
                step = 3;
            break;

            case 3:
                mutate_population(0, 0.15, 0);
                step = 0;
            break;
        }
    }

  /* This is necessary to cleanup webots resources */
    wb_robot_cleanup();
    wbu_driver_cleanup();
    wb_supervisor_simulation_reset();

    return 0;
}
