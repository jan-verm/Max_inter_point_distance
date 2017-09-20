/*
 * genetic.h
 *
 *  Created on: 14-okt.-2014
 *      Author: Jan Vermeulen
 */

#ifndef GENETIC_H_
#define GENETIC_H_

#include "geometry.h"

typedef struct {
	int size;
	Point* points;
	float fitness;
} Individual;

typedef struct {
	Individual* individuals;
} Population;

float fitness(Individual* individual);

float fitness_point(Point* p, Individual* individual);

void init_population(Population* population, int population_size, Polygon* polygon, int points);

void cross_over(Individual* i1, Individual* i2, Individual* c1, Individual* c2);

void mutate(Polygon* polygon, Individual* i, float max_mutation_step);

Individual** select_individuals(Polygon* polygon, Population* population, int population_length, int n);

void print_individual(Individual* i);

#endif
