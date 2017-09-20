/*
 * mpi.h
 *
 *  Created on: 14-nov.-2014
 *      Author: Jan Vermeulen
 */

#ifndef MPITOOLS_H_
#define MPITOOLS_H_

#include "genetic.h"


typedef struct {
	float value;
	int rank;
} Maxlock_struct;

float* flatten_population(int points, Population* population, int population_size);

void unflatten_population(float* flat, int points, Population* population, int population_size);

 #endif