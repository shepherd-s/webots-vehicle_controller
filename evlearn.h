/**
 * File: evlearn.h
 * Description: This is a header for a genetic algorithm
 *              using a weight matrix as chromosome.
 *
 * Author: Shepherdsoft_s
 * Contact: shepherdsoft@outlook.com
 */

#pragma once

#define CHROM_ARRAY_SIZE 6       //TO CHANGE
#define CHROM_SIZE 9             //
#define POPULATION_SIZE 20      //

typedef struct Individual {
    int s_count;
    double fitness;

    double chromosome[CHROM_ARRAY_SIZE][CHROM_SIZE];
} Individual;

//Gives ramdom initial values to the chromosomes of the population
void initialize_population(int index);

//The evaluation is done througt an effective_evaluation function
//wich has to be implemented in the code using this header.
void evaluate_population(int index, void (*effective_evaluation) (int i));

//Binary ranking
void select_population(int index, int k);

//BLX-alpha
void cross_population();

//Uniform mutation
void mutate_population(int index, double m_prob, int best);

//This is to store the last generation so the
//trainning time is not loss
void write_file(int index, int generation);
