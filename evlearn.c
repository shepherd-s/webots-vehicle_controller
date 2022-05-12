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
 * This 2d array contains the limits of the chromosome values so that the
 * initialize population function provides a random value for each chromosome
 * component between the given range. There are two vectors for each chromosome
 * the first one contains the lower limit and the second the upper limit.
 * They should be changed depending of the specific application.
 * It has to ve declared and initialized in the controller code.
 */
extern const double min_max[CHROM_ARRAY_SIZE*2][CHROM_SIZE];

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
int find_best()
{
    int index = 0;
    double i_f = 0;
    double index_f = 0;

    for (int i=1; i<POPULATION_SIZE; i++) {
        i_f = population[i].fitness;
        index_f = population[index].fitness;
        if(i_f > index_f) {
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
int next_alive(int begin)
{
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
 * @param chromosome1 subtrahend vector
 * @param chromosome2 minuend vector
 * @return the positive or negative distance
 */
double euclidean_d(double chromosome1[CHROM_ARRAY_SIZE][CHROM_SIZE],
                   double chromosome2[CHROM_ARRAY_SIZE][CHROM_SIZE])
{
    double sum = 0;
    double sub = 0;

    for (int i=0; i<CHROM_ARRAY_SIZE; i++) {
        for (int j=0; j<CHROM_SIZE; j++) {
            sub = chromosome1[i][j] - chromosome2[i][j];
            sum += pow(sub, 2);
        }
    }
    return sqrt(sum);
}

/**
 * Writes the chromosomes and fitness of each individual
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
        for (int i=0; i<CHROM_ARRAY_SIZE; i++) {
            for (int j=0; j<CHROM_SIZE; j++) {
                fprintf(file, "%.4f ", population[index].chromosome[i][j]);
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

int contains(int index_a[], int k, int r)
{
    for (int i=0; i<k; i++) {
        if (index_a[i] == r) {
            return 1;
        }
    }
    return 0;
}

/**
 * Uniform crossover
 *
 * @brief cross_chromosomes
 * @param son_matrix matrix storing all generated offspring
 * @param son_index current son
 * @param mother
 * @param father
 */
void cross_chromosomes(double son_matrix[POPULATION_SIZE][CHROM_ARRAY_SIZE][CHROM_SIZE],
                      int son_index, int mother, int father)
{
    int select = rand()%2;

    for (int i=0; i<CHROM_ARRAY_SIZE; i++) {
        for (int j=0; j<CHROM_SIZE; j++)  {
            double chrom_val = population[mother].chromosome[i][j];
            if (select) {
                chrom_val = population[father].chromosome[i][j];
            }
            son_matrix[son_index][i][j] = chrom_val;
            select = rand()%2;
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
    
        for (int i = 0; i < CHROM_ARRAY_SIZE; i++) {
            for (int j = 0; j < CHROM_SIZE; j++) {
                double random = f_rand(min_max[i*2][j], min_max[i*2+1][j]);
                population[index].s_count = 0;
                population[index].fitness = 0;
                population[index].chromosome[i][j] = random;
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
void evaluate_population(int index, void (*effective_evaluation) (int i))
{
    effective_evaluation(index);
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
    int index_a[k];
    int winner = 0;
    double index_f = 0;
    double winner_f = 0;
    int r = (int)f_rand(0, POPULATION_SIZE);

    for (int i=0; i<k; i++) {
        index_a[i] = -1;
    }

    if (r == POPULATION_SIZE) {
        r--;
    }
    index_a[0] = r;

    for (int i=1 ; i<k; i++) {
        r = (int)f_rand(0, POPULATION_SIZE);
        if (r == POPULATION_SIZE) {
            r--;
        }
        while (contains(index_a, k, r)) {
            r = (int)f_rand(0, POPULATION_SIZE);
            if (r == POPULATION_SIZE) {
                r--;
            }
        }
        index_a[i] = r;
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
        select_population(index+1, k);
    }
}

/**
 * Implements the crossover method of the genetic algorithm.
 * The specific method is Uniform crossover. The substitution is done
 * in this step as well (with elitism k = 1).
 *
 * @brief cross_population
 */
void cross_population()
{
    int best = find_best();
    int mother = 0;
    int s_count = 0;
    int son_index = 0;
    int father = 0;
    double son_matrix[POPULATION_SIZE][CHROM_ARRAY_SIZE][CHROM_SIZE] = {};

    for (int i=0; i<POPULATION_SIZE; i++) {
        mother = i;
        s_count = population[mother].s_count;

        if (s_count > 0) {
            for (int j=0; j<s_count; j++) {
                father = next_alive(mother+j+1);
                cross_chromosomes(son_matrix, son_index, mother, father);
                son_index++;
            }
        }
    }

    for (int i=0; i<POPULATION_SIZE; i++) {
        if (i!=best) {
            for (int j=0; j<CHROM_ARRAY_SIZE; j++) {
                for (int k=0; k<CHROM_SIZE; k++) {
                    population[i].chromosome[j][k] = son_matrix[i][j][k];
                }
            }
        }
        population[i].s_count = 0;
    }
}

/**
 * Implements the mutation method of the genetic algorithm.
 * Uniform mutation inversely proportional to the fitness.
 *
 * @brief mutate_population
 * @param index recursive index
 */
void mutate_population(int index, double m_prob, int best)
{
    int b = index == 0 ? find_best() : best;
    double mp = m_prob;
    double best_f = population[b].fitness;
    double omega = (1-(population[index].fitness/best_f))*mp;
    double threshold = 0;

    if (index < POPULATION_SIZE) {
        for (int i=0; i<CHROM_ARRAY_SIZE; i++) {
            if (i!=b) {
                for (int j=0; j<CHROM_SIZE; j++) {
                    threshold = f_rand(0, 1);
                    if (threshold < omega) {
                        population[index].chromosome[i][j] = f_rand(min_max[i*2][j], min_max[i*2+1][j]);
                    }
                }
            }
            population[i].fitness = 0;
        }
        mutate_population(index+1, mp, b);
    }
}
