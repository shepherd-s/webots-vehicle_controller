/**
 * File: evlearn.h
 * Description: This is a header for a genetic algorithm
 *              using a weight matrix as chromosome.
 *
 * Author: shepherd-s
 * Contact: shepherdsoft@outlook.com
 */

#pragma once

#define MAX_CHROM_ARRAY_SIZE 100
#define MAX_CHROM_SIZE 100
#define MAX_POPULATION_SIZE 100

typedef struct Individual {
    int s_count;
    double fitness;

    double chromosome[MAX_CHROM_ARRAY_SIZE][MAX_CHROM_SIZE];
} Individual;

// Initializes the algorithm. It is mandatory.
int 
init(size_t population_size, size_t chrom_array_size, size_t chrom_size, double** min_max_matrix);

// Gives ramdom initial values to the chromosomes of the population..
void initialize_population(int index);

// The evaluation is done througt an effective_evaluation function
// wich has to be implemented in the code using this header.
void evaluate_population(int index, void (*effective_evaluation) (int i));

// Tournament selection with k individuals.
void select_population(int index, int k);

// Uniform crossover with elitism k = 1.
void cross_population();

// Uniform mutation with dynamic probability (inversely proportional to fitness).
void mutate_population(int index, double m_prob, int best);

// This is to store the last generation so the
// trainning time is not loss.
void write_file(int index, int generation);

// Checks if the value given is out of the {min, max} and returns it truncated.
double truncate_value(double value, double min, double max);
