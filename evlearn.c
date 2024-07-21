/**
 * File: evlearn.c
 * Description: This is the implementation of the
 *              evlearn.h header.
 *
 * Author: shepherd-s
 * Contact: shepherdsoft@outlook.com
 */

#include "evlearn.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * This variables are the relevant user defined sizes, 
 * the population itself and the pointer to the file 
 * that stores the chromosomes of the best generation, 
 * so that it can be reused. The population will be 
 * initialized to random values or to the values in the
 * best generation on the file. 
 */
size_t evlearn_population_size;
size_t evlearn_chrom_array_size;
size_t evlearn_chrom_size;
Individual evlearn_population[MAX_POPULATION_SIZE];
FILE *evlearn_best_gen_file;

/**
 * This 2d array contains the limits of the chromosome values so that the
 * initialize population function provides a random value for each chromosome
 * component within the given range. There are two vectors for each chromosome,
 * the first one contains the lower limit and the second the upper limit.
 * They should be changed depending of the specific application. By default, the
 * init function initializes every even vector to -1, (min_value) and every odd 
 * to 1 (max_value).
 * Example:
 * Chromosome -> [[0, 0, 0],  min_max -> [[-1, -1, -1],
 *                [0, 0, 0]]              [ 1,  1,  1],
 *                                        [-1, -1, -1],
 *                                        [ 1,  1,  1]]
 * Note that this would be the default min max matrix but elements of every vector
 * can be different.
 */
double evlearn_min_max_matrix[MAX_CHROM_ARRAY_SIZE*2][MAX_CHROM_SIZE];

/**
 * \brief This is to initialize the sizes and the max, min matrix.
 * The algorithm assumes this function is called, so it is
 * mandatory to call it before starting.
 * 
 * \param population_size The number of individuals in the population.
 * \param chrom_array_size The number of rows of the chromosome matrix,
 * it is presented as a matrix. Every row represents a characteristic
 * of the system, so for example, in a self driving vehicle they can
 * codify how the vehicle change the gears, how it throttle, and so on.
 * \param chrom_size The number of chromosome components in every row. 
 * As an example, they can codify how the steering weel reacts to every 
 * bundle of an obstacle detection LiDAR sensor, or what the throttle 
 * should be, etc.
 * \param min_max_matrix The matrix that sets the min and max values
 * for every row in the chromosome matrix. If NULL is passed, it
 * initializes even rows to -1 and odd to 1, so that every chromosome
 * component can have a value within the range {-1, 1}.
 * 
 * \return Returns 0 for success, 1 otherwise.
 */
int init(
    size_t population_size, 
    size_t chrom_array_size, 
    size_t chrom_size, 
    double** min_max_matrix)
{
    int condition = 
    population_size <= MAX_POPULATION_SIZE && 
    chrom_array_size <= MAX_CHROM_ARRAY_SIZE && 
    chrom_size <= MAX_CHROM_SIZE;

    if (condition)
    {
        evlearn_population_size = population_size;
        evlearn_chrom_array_size = chrom_array_size;
        evlearn_chrom_size = chrom_size;

        for (int i = 0; i < chrom_array_size * 2; i++)
        {
            for (int j = 0; j < chrom_array_size * 2; j++)
            {
                if (min_max_matrix != NULL)
                {
                    evlearn_min_max_matrix[i][j] = min_max_matrix[i][j];
                }
                else
                {
                    evlearn_min_max_matrix[i][j] = i % 2  == 0 ? -1 : 1;
                }
            }
        }
    
        return 0;
    }

    return 1;
}

/**
 * \brief This is to find a random double value within a given
 * interval.
 * 
 * \param min minimum value returned
 * \param max maximum value returned
 * 
 * \return it returns a random value within the interval
 */
double f_rand(double min, double max)
{
    double f = (double)rand()/RAND_MAX;
    return min + f * (max - min);
}

/**
 * \brief This finds the individual with the best fitness
 *
 * \return the index of the best individual
 */
int find_best()
{
    int index = 0;
    double i_f = 0;
    double index_f = 0;

    for (int i = 1; i < evlearn_population_size; i++) {
        i_f = evlearn_population[i].fitness;
        index_f = evlearn_population[index].fitness;
        if(i_f > index_f) {
            index = i;
        }
    }

    return index;
}

/**
 * \brief Finds the next individual alive (with a value different from
 * 0 in the field "s_count"), beginning from "begin" parameter.
 *
 * \param begin the beginning index
 * 
 * \return the index of the next alive individual
 */
int next_alive(int begin)
{
    int index = begin;
    int count = 0;

    while (count <= evlearn_population_size) {
        if (index == evlearn_population_size) {
            index = 0;
        }
        if (evlearn_population[index].s_count > 0) {
            return index;
        }
        count++;
        index++;
    }
    return -1;
}

/**
 * \brief Calculates the euclidean distance of two vectors.
 *
 * \param chromosome1 subtrahend vector
 * \param chromosome2 minuend vector
 * 
 * \return the positive or negative distance
 */
double euclidean_d(double** chromosome1, double** chromosome2)
{
    double sum = 0;
    double sub = 0;

    for (int i = 0; i < evlearn_chrom_array_size; i++) {
        for (int j = 0; j < evlearn_chrom_size; j++) {
            sub = chromosome1[i][j] - chromosome2[i][j];
            sum += pow(sub, 2);
        }
    }
    return sqrt(sum);
}

/**
 * \brief Writes the chromosomes and fitness of each individual
 * in the population to a file called best_gen.txt.
 *
 * \param index recursive index, should start in 0
 * \param generation the current generation
 */
void write_file(int index, int generation)
{
    if (index == 0) {
        evlearn_best_gen_file = fopen("best_gen.txt", "w+");
    }

    if (index < evlearn_population_size) {
        for (int i = 0; i < evlearn_chrom_array_size; i++) {
            for (int j = 0; j < evlearn_chrom_size; j++) {
                fprintf(evlearn_best_gen_file, "%.4f ", evlearn_population[index].chromosome[i][j]);
            }
            fprintf(evlearn_best_gen_file, "\n");
        }
        fprintf(evlearn_best_gen_file, "%.4f\n", evlearn_population[index].fitness);
        write_file(index+1, generation);
    }
    else {
        fprintf(evlearn_best_gen_file, "Generation: %d", generation);
        fclose(evlearn_best_gen_file);
    }
}

void read_file()
{

}

/**
 * \brief Checks if an element exists in an array
 *
 * \param index_a array to check
 * \param k size of the array
 * \param r element to find
 * 
 * \return 1 if the element r exists in index_a, 0 otherwise
 */
int contains(int* index_a, int k, int r)
{
    for (int i = 0; i < k; i++) {
        if (index_a[i] == r) {
            return 1;
        }
    }
    return 0;
}

/**
 * \brief Uniform crossover
 *
 * \param son_matrix matrix storing all generated offspring
 * \param son_index current son
 * \param mother
 * \param father
 */
void cross_chromosomes(double*** son_matrix, int son_index, int mother, int father)
{
    int select = rand()%2;

    for (int i = 0; i < evlearn_chrom_array_size; i++) {
        for (int j = 0; j < evlearn_chrom_size; j++)  {
            double chrom_val = evlearn_population[mother].chromosome[i][j];
            if (select) {
                chrom_val = evlearn_population[father].chromosome[i][j];
            }
            son_matrix[son_index][i][j] = chrom_val;
            select = rand()%2;
        }
    }
}

/**
 * \brief Gives random values within the given limits in the evlearn_min_max_matrix
 * to the population. Fitness is initialized to 0.
 *
 * \param index recursive index
 */
void initialize_population(int index)
{
    if (index < evlearn_population_size) {
    
        for (int i = 0; i < evlearn_chrom_array_size; i++) {
            for (int j = 0; j < evlearn_chrom_size; j++) {
                double random = f_rand(evlearn_min_max_matrix[i*2][j], evlearn_min_max_matrix[i*2+1][j]);
                evlearn_population[index].s_count = 0;
                evlearn_population[index].fitness = 0;
                evlearn_population[index].chromosome[i][j] = random;
            }
        }
    initialize_population(index+1);
    }
}

/**
 * \brief A pointer to the actual evaluation function wich has to be
 * implemented in the application using evlearn.h.
 *
 * \param index the index of the individual for each iteration.
 */
void evaluate_population(int index, void (*effective_evaluation) (int i))
{
    effective_evaluation(index);
}

/**
 * \brief Implements the selection method of the genetic algorithm.
 * A tournament with k given by the argument k.
 *
 * \param index recursive index
 * \param k number of individuals in the tournament
 */
void select_population(int index, int k)
{
    int winner = 0;
    double index_f = 0;
    double winner_f = 0;
    int* index_a = (int*) malloc(k * sizeof(int)) ;
    int r = (int)f_rand(0, evlearn_population_size);

    for (int i = 0; i < k; i++) {
        index_a[i] = -1;
    }

    if (r == evlearn_population_size) {
        r--;
    }
    index_a[0] = r;

    for (int i = 1 ; i < k; i++) {
        r = (int)f_rand(0, evlearn_population_size);
        if (r == evlearn_population_size) {
            r--;
        }
        while (contains(index_a, k, r)) {
            r = (int)f_rand(0, evlearn_population_size);
            if (r == evlearn_population_size) {
                r--;
            }
        }
        index_a[i] = r;
    }
    winner_f = evlearn_population[index_a[0]].fitness;

    for (int i = 1; i < k; i++) {
        index_f = evlearn_population[index_a[i]].fitness;
        if (index_f > winner_f) {
            winner = index_a[i];
        }
    }
    evlearn_population[winner].s_count++;

    if(index < evlearn_population_size - 1) {
        select_population(index + 1, k);
    }

    free(index_a);
}

/**
 * \brief Implements the crossover method of the genetic algorithm.
 * The specific method is Uniform crossover. The substitution is done
 * in this step as well (with elitism k = 1).
 */
void cross_population()
{
    int best = find_best();
    int mother = 0;
    int s_count = 0;
    int son_index = 0;
    int father = 0;
    double son_matrix[MAX_POPULATION_SIZE][MAX_CHROM_ARRAY_SIZE][MAX_CHROM_SIZE] = {0};

    for (int i = 0; i < evlearn_population_size; i++) {
        mother = i;
        s_count = evlearn_population[mother].s_count;

        if (s_count > 0) {
            for (int j = 0; j < s_count; j++) {
                father = next_alive(mother + j + 1);
                cross_chromosomes(son_matrix, son_index, mother, father);
                son_index++;
            }
        }
    }

    for (int i = 0; i < evlearn_population_size; i++) {
        if (i != best) {
            for (int j = 0; j < evlearn_chrom_array_size; j++) {
                for (int k = 0; k < evlearn_chrom_size; k++) {
                    evlearn_population[i].chromosome[j][k] = son_matrix[i][j][k];
                }
            }
        }
        evlearn_population[i].s_count = 0;
    }
}

/**
 * \brief Implements the mutation method of the genetic algorithm.
 * Uniform mutation inversely proportional to the fitness.
 *
 * \param index recursive index
 * \param m_prob max mutation probability
 * \param best current best individual
 */
void mutate_population(int index, double m_prob, int best)
{
    int b = index == 0 ? find_best() : best;
    double mp = m_prob;
    double best_f = evlearn_population[b].fitness;
    double omega = (1 - (evlearn_population[index].fitness / best_f)) * mp;
    double threshold = 0;

    if (index < evlearn_population_size) {
        for (int i = 0; i < evlearn_chrom_array_size; i++) {
            if (i != b) {
                for (int j = 0; j < evlearn_chrom_size; j++) {
                    threshold = f_rand(0, 1);
                    if (threshold < omega) {
                        evlearn_population[index].chromosome[i][j] = 
                        f_rand(evlearn_min_max_matrix[i * 2][j], evlearn_min_max_matrix[i * 2 + 1][j]);
                    }
                }
            }
            evlearn_population[i].fitness = 0;
        }
        mutate_population(index + 1, mp, b);
    }
}

/**
 * \brief Check if the value given is out of the {min, max}
 * interval.
 *
 * \param value the value to check
 * \param min minimum returned
 * \param max maximum returned
 * \return a value within the interval
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