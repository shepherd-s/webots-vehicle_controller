/**
 * File: evlearn.h
 * Description: This is a header for a genetic algorithm
 *              using a weight matrix as chromosome.
 *
 * Author: shepherd-s
 * Contact: shepherdsoft@outlook.com
 */

#include <stddef.h>

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
int init(
    size_t population_size, 
    size_t chrom_array_size, 
    size_t chrom_size, 
    double** min_max_matrix,
    char* file_path);

// Main function of the algorithm.
void 
compute_next_generation(size_t tournament_size, double mutation_probability);

// Gets the best individual so far.
Individual get_best();

// HELPERS ////////////////////////////////////////////////////////////////////////////////////////

// This is to store the last generation of a traning. If you want to store the generated population,
// you need to call this function explicitly.
int write_file(int index, size_t generation);

// Checks if the value given is out of the {min, max} and returns it truncated.
double truncate_value(double value, double min, double max);

// Get a random value in the range {min, max}
double f_rand(double min, double max);
