/*
 * genetic.c
 *
 *  Created on: 14-okt.-2014
 *      Author: jan
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "genetic.h"
#include "geometry.h"


float fitness(Individual* population) {
	int i;
	float fitness;
	fitness = 0.0;
	for (i = 0; i < population->size; i++) {
		fitness += fitness_point(&(population->points[i]), population);
	}
	return fitness;
}

float fitness_point(Point* p, Individual* population) {
	int i;
	float fitness, buffer;
	fitness = 0.0;
	for (i = 0; i < population->size; i++) {
		buffer = sqrtf(distance(population->points[i], *p));
		fitness += buffer;
	}
	return fitness;
}

void init_population(Population* population, int population_size, Polygon* polygon, int points) {
	int i, j, x;
	float total_surface, r, r1, r2;
	float *surface_array;
	surface_array = (float*) malloc((points - 2)*sizeof(float));

	// Calculate surface of #points-2 triangles
	total_surface = 0;
	for (i = 1; i < points - 2; i++) {
		surface_array[i-1] = triangle_surface(&(polygon->points[0]),
				&(polygon->points[i]), &(polygon->points[i + 1]));
		total_surface += surface_array[i-1];
	}

	for (x = 0; x < population_size; x++) {
		population->individuals[x].points = (Point*) malloc(points * sizeof(Point));
		population->individuals[x].size = points;

		for (i = 0; i < points; i++) {
			// Random float between 0 and total_surface
			r = ((float) rand() / (float) (RAND_MAX)) * total_surface;

			// Select triangle
			j = 1;
			while (r >= surface_array[j-1]) {
				r -= surface_array[j];
				j++;
			}

			// Generate point inside triangle (0, j, j+1)
			r1 = ((float) rand() / (float) (RAND_MAX));
			r2 = ((float) rand() / (float) (RAND_MAX));
			population->individuals[x].points[i].x = (1 - sqrtf(r1))
					* polygon->points[0].x
					+ (sqrtf(r1) * (1 - r2)) * polygon->points[j].x
					+ (sqrtf(r1) * r2) * polygon->points[j + 1].x;
			population->individuals[x].points[i].y = (1 - sqrtf(r1))
					* polygon->points[0].y
					+ (sqrtf(r1) * (1 - r2)) * polygon->points[j].y
					+ (sqrtf(r1) * r2) * polygon->points[j + 1].y;
		}
		population->individuals[x].fitness = fitness(&population->individuals[x]);
	}
	free(surface_array);
}

void cross_over(Individual* i1, Individual* i2, Individual* c1, Individual* c2) {
	int r, i, size;

	size = i1->size;
	r = rand() % size;

	c1->points = (Point*) malloc(size * sizeof(Point));
	c2->points = (Point*) malloc(size * sizeof(Point));
	c1->size = c2->size = size;

	for (i = 0; i < r; i++) {
		c1->points[i].x = i1->points[i].x;
		c1->points[i].y = i1->points[i].y;
		c2->points[i].x = i2->points[i].x;
		c2->points[i].y = i2->points[i].y;
	}
	for (i = r; i < size; i++) {
		c1->points[i].x = i2->points[i].x;
		c1->points[i].y = i2->points[i].y;
		c2->points[i].x = i1->points[i].x;
		c2->points[i].y = i1->points[i].y;
	}

}

void mutate(Polygon* polygon, Individual* i, float max_mutation_step) {
	int index;
	float dx, dy, bufferx, buffery;
	index = ((float) rand() / (float) (RAND_MAX)) * i->size;
	dx = ((float) rand() / (float) (RAND_MAX)) * (2*max_mutation_step)
			- max_mutation_step;
	dy = ((float) rand() / (float) (RAND_MAX)) * (2*max_mutation_step)
			- max_mutation_step;
	bufferx = i->points[index].x;
	buffery = i->points[index].y;
	i->points[index].x += dx;
	i->points[index].y += dy;
	if( !inside(polygon, i->points[index]) ) {
		i->points[index].x = bufferx;
		i->points[index].y = buffery;
	}
}

Individual** select_individuals(Polygon* polygon, Population* population, int population_length, int n) {
	float* cumulative_fitness_array;
	float total_fitness, offset, distance, buffer;
	int j, c, r;
	Individual** collection;
	Individual* temp;

	// Stochastic universal sampling
	cumulative_fitness_array = (float*) malloc(population_length * sizeof(float));
	total_fitness = 0;
	for (j = 0; j < population_length; j++) {
		cumulative_fitness_array[j] = total_fitness + population->individuals[j].fitness;
		total_fitness = cumulative_fitness_array[j];
	}
	collection = (Individual**) malloc(n * sizeof(Individual*));
	distance = total_fitness / n;
	offset = ((float) rand() / (float) (RAND_MAX)) * total_fitness;
	for (c = 0; c < n; c++) {
		buffer = (offset + ((float) c) * distance);
		if(buffer>total_fitness) {
			buffer -= total_fitness;
		}
		for (j = 0; buffer > cumulative_fitness_array[j]; j++) {}
		collection[c] = &population->individuals[j];
	}
	free(cumulative_fitness_array);

	// Shuffle collection
	for (j = n-1; j >= 0; --j){
	    r = rand() % (j+1);
	    temp = collection[j];
	    collection[j] = collection[r];
	    collection[r] = temp;
	}

	return collection;
}

void print_individual(Individual* i) {
	int x;
	for(x=0; x<i->size; x++) {
		printf("(%.1f, %.1f)", i->points[x].x, i->points[x].y);
	}
	printf(") fitness: %f \n", i->fitness);
}
