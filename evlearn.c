/**
 * File: evlearn.c
 * Description: This is the implementation of the
 *              evlearn.h header.
 *
 * Author: Shepherdsoft_s
 * Contact: shepherdsoft@outlook.com
 */

#include "evlearn.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * This 2 next variables have to be declared in the controller
 * code using this evlearn implementation.
 */
extern Individual population[POPULATION_SIZE];
extern FILE *file;

/**
 * This 2d array contains the limits of the cromosome values so that the
 * initialize population function provides a random value for each cromosome
 * component between the given range. There are two vectors for each cromosome
 * the first one contains the lower limit and the second the upper limit.
 * They should be changed depending of the specific application.
 * It has to ve declared and initialized in the controller code.
 */
extern const double min_max[CROM_ARRAY_SIZE*2][CROM_SIZE];

/**
 * This is to find a random double value within a given
 * interval.
 *
 * @brief f_rand
 * @param min minimum value returned
 * @param max maximum value returned
 * @return it returns a random value within the interval
 */
double f_rand(double min, double max)
{
    double f = (double)rand()/RAND_MAX;
    return min + f * (max - min);
}

/**
 * This find the individual with the best fitness
 *
 * @brief find_best
 * @return the index of the best individual
 */
int find_best() {
    int index = 0;

    for (int i=0; i<POPULATION_SIZE; i++) {
        if(population[i].fitness > population[index].fitness) {
            index = i;
        }
    }
    return index;
}

/**
 * Finds the next individual alive (with a value different from
 * 0 in the field "s_count"), beginning from "begin" parameter.
 *
 * @brief next_alive
 * @param begin the beginning index
 * @return the index of the next alive individual
 */
int next_alive(int begin) {
    int index = begin;
    int count = 0;

    while (count <= POPULATION_SIZE) {
        if (index == POPULATION_SIZE) {
            index = 0;
        }
        if (population[index].s_count > 0) {
            return index;
        }
        count++;
        index++;
    }
    return -1;
}

/**
 * Calculates the euclidean distance of two vectors.
 *
 * @brief euclidean_d
 * @param cromosome1 subtrahend vector
 * @param cromosome2 minuend vector
 * @return the positive or negative distance
 */
double euclidean_d(double cromosome1[CROM_ARRAY_SIZE][CROM_SIZE],
                   double cromosome2[CROM_ARRAY_SIZE][CROM_SIZE])
{
    double sum = 0;
    double sub = 0;

    for (int i=0; i<CROM_ARRAY_SIZE; i++) {
        for (int j=0; j<CROM_SIZE; j++) {
            sub = cromosome1[i][j] - cromosome2[i][j];
            sum += pow(sub, 2);
        }
    }

    return sqrt(sum);
}

/**
 * Writes the cromosomes and fitness of each individual
 * in the population to a file called best_gen.txt.
 *
 * @brief write_file
 * @param index recursive index
 */
void write_file(int index)
{
    if (index == 0) {
        file = fopen("best_gen.txt", "w+");
    }

    if (index < POPULATION_SIZE) {
        for (int i=0; i<CROM_ARRAY_SIZE; i++) {
            for (int j=0; j<CROM_SIZE; j++) {
                fprintf(file, "%.4f ", population[index].cromosome[i][j]);
            }
            fprintf(file, "\n");
        }
        fprintf(file, "%.4f\n", population[index].fitness);
        write_file(index+1);
    }
    else {
        fclose(file);
    }
}

/**
 * Performs a BLX-apha crossing.
 *
 * @brief cross_cromosomes
 * @param mother index of the mother
 * @param father index of the father
 */
void cross_cromosomes(double son_matrix[POPULATION_SIZE][CROM_ARRAY_SIZE][CROM_SIZE],
                      int son_index, int mother, int father)
{
    double min = 0;
    double max = 0;
    double l_limit = 0;
    double u_limit = 0;
    double alpha = 0;
    double euclidean_1_m = 0;
    double euclidean_2_m = 0;
    double crowdfunding1[CROM_ARRAY_SIZE][CROM_SIZE] = {};
    double crowdfunding2[CROM_ARRAY_SIZE][CROM_SIZE] = {};

    alpha = f_rand(0, 1);

    for (int i=0; i<CROM_ARRAY_SIZE; i++) {
        for (int j=0; j<CROM_SIZE; j++)  {
            if (population[mother].cromosome[i][j] >= population[father].cromosome[i][j]) {
                max = population[mother].cromosome[i][j];
                min = population[father].cromosome[i][j];
            }
            else {
                min = population[mother].cromosome[i][j];
                max = population[father].cromosome[i][j];
            }

            l_limit = min - alpha*(min-min_max[i*2][j]);
            u_limit = max + alpha*(min_max[i*2+1][j]-max);

            crowdfunding1[i][j] = f_rand(l_limit, u_limit);
            crowdfunding2[i][j] = f_rand(l_limit, u_limit);
        }
    }

    euclidean_1_m = (double)fabs(euclidean_d(crowdfunding1, population[mother].cromosome));
    euclidean_2_m = (double)fabs(euclidean_d(crowdfunding2, population[mother].cromosome));

    if (euclidean_1_m > euclidean_2_m) {
        for (int i=0; i<CROM_ARRAY_SIZE; i++) {
            for (int j=0; j<CROM_SIZE; j++) {
                son_matrix[son_index][i][j] = crowdfunding1[i][j];
            }
        }
    }
    else {
        for (int i=0; i<CROM_ARRAY_SIZE; i++) {
            for (int j=0; j<CROM_SIZE; j++) {
                son_matrix[son_index][i][j] = crowdfunding2[i][j];
            }
        }
    }
}

/**
 * Gives random values within the given limits in the min_max matrix
 * to the population. Fitness is initialized to 0.
 *
 * @brief initialize_population
 * @param index recursive index
 */
void initialize_population(int index)
{
    if (index < POPULATION_SIZE) {
    
        for (int i = 0; i < CROM_ARRAY_SIZE; i++) {
            for (int j = 0; j < CROM_SIZE; j++) {
                double random = f_rand(min_max[i*2][j], min_max[i*2+1][j]);
                population[index].s_count = 0;
                population[index].fitness = 0;
                population[index].cromosome[i][j] = random;
            }
        }
    initialize_population(index+1);
    }
}

/**
 * A pointer to the actual evaluation function wich has to be
 * implemented in the application using evlearn.h.
 *
 * @brief evaluate_population
 * @param index the index of the individual for each iteration.
 */
void evaluate_population(int index, void (*efective_evaluation) (int i))
{
    efective_evaluation(index);
}

/**
 * Implements the selection method of the genetic algorithm.
 * A tournament with k given by the argument k.
 *
 * @brief select_population
 * @param index recursive index
 * @param k number of individuals in the tournament
 */
void select_population(int index, int k)
{
    int winner = 0;
    int index_a[] = {};
    double index_f = 0;
    double winner_f = 0;

    for (int i=0 ; i<k; i++) {
        index_a[i] = (int)f_rand(0, POPULATION_SIZE);
    }
    for (int i=0; i<k; i++) {
        if (index_a[i] == POPULATION_SIZE) {
            index_a[i]--;
        }
    }
    winner_f = population[index_a[0]].fitness;
    for (int i=1; i<k; i++) {
        index_f = population[index_a[i]].fitness;
        if (index_f > winner_f) {
            winner = index_a[i];
        }
    }
    population[winner].s_count++;

    if(index < POPULATION_SIZE-1) {
        select_population(index+1);
    }
}

/**
 * Implements the crossing method of the genetic algorithm.
 * The specific method is BLX-alpha. The substitution is done
 * in this step as well.
 *
 * @brief cross_population
 */
void cross_population()
{
    int mother = 0;
    int s_count = 0;
    int son_index = 0;
    int father = 0;
    double son_matrix[POPULATION_SIZE][CROM_ARRAY_SIZE][CROM_SIZE] = {};

    for (int i=0; i<POPULATION_SIZE; i++) {
        mother = i;
        s_count = population[mother].s_count;

        if (s_count > 0) {
            for (int j=0; j<s_count; j++) {
                father = next_alive(mother+1);
                cross_cromosomes(son_matrix, son_index, mother, father);
                son_index++;
            }
        }
    }

    for (int i=0; i<POPULATION_SIZE; i++) {
        for (int j=0; j<POPULATION_SIZE; j++) {
            for (int k=0; k<POPULATION_SIZE; k++) {
                population[i].s_count = 0;
                population[i].fitness = 0;
                population[i].cromosome[j][k] = son_matrix[i][j][k];
            }
        }
    }
}

/**
 * Implements the mutation method of the genetic algorithm.
 * Uniform mutation.
 *
 * @brief mutate_population
 * @param index recursive index
 */
void mutate_population(int index)
{
    double omega = 0;

    if (index < POPULATION_SIZE) {
        for (int i=0; i<CROM_ARRAY_SIZE; i++) {
            for (int j=0; j<CROM_SIZE; j++) {
                omega = f_rand(0, 1);
                if (omega < min_max[CROM_ARRAY_SIZE*2-1][1]) {
                    population[index].cromosome[i][j] = f_rand(min_max[i*2][j], min_max[i*2+1][j]);
                }
            }
        }
        mutate_population(index+1);
    }
}
