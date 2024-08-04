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
int init(
    size_t population_size, 
    size_t chrom_array_size, 
    size_t chrom_size, 
    double** min_max_matrix,
    char* file_path);

// Main function of the algorithm.
void 
compute_next_generation(size_t tournament_size, double mutation_probability, size_t write_flag);

// Gets the best individual so far.
Individual get_best();

// HELPERS ////////////////////////////////////////////////////////////////////////////////////////

// This is to store the last generation of a traning.
int write_file(size_t index, size_t generation);

// Read previously generated file.
int read_file(char* file_path);

// Checks if the value given is out of the {min, max} and returns it truncated.
double truncate_value(double value, double min, double max);

// Get a random value in the range {min, max}
double f_rand(double min, double max);
