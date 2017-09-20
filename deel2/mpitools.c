/*
 * mpi.c
 *
 *  Created on: 14-nov.-2014
 *      Author: Jan Vermeulen
 */

#include "mpitools.h"
#include <stdlib.h>
#include <stdio.h>

float* flatten_population(int points, Population* population, int population_size) {
	int i, j, index;
	float* flat;
	
	flat = (float*) malloc(population_size*points*2*sizeof(float));
	index = 0;
	for(i=0; i < population_size; i++) {
		for(j=0; j<points; j++) {
			flat[index] = population->individuals[i].points[j].x;
			flat[index+1] = population->individuals[i].points[j].y;
			index+=2;
		}
	}

	return flat;
}

void unflatten_population(float* flat, int points, Population* population, int population_size) {
	int index, i, j;
	index = 0;

	for(i=0; i < population_size; i++) {
		for(j=0; j<points; j++) {
			population->individuals[i].points[j].x = flat[index];
			population->individuals[i].points[j].y = flat[index+1];
			index +=2;
		}
	}
}