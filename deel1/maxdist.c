/*
 * maxdist.c
 *
 *  Created on: 13-okt.-2014
 *      Author: jan
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "geometry.h"
#include "io.h"
#include "genetic.h"

double getTimeElapsed(struct timeval end, struct timeval start)
{
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.00;
}

int main(int argc, char* argv[]) {
	Polygon* polygon;
	Population *population, *population_buffer;
	int points, i, j, c, best_i;
	float best_fitness, buffer, r, total_fitness;
	Individual **parent_collection, **selection;

	int iterations, population_size, num_crossovers;
	float max_mutation_step, mutation_prob;

	double time_elapsed;
	struct timeval start, end;

	if (argc == 3) {
		// Set default args
		iterations=10000;
		population_size=400;
		max_mutation_step=0.9;
		num_crossovers=100;
		mutation_prob=0.5;
	}
	else if (argc == 8) {
		iterations=atoi(argv[3]);;
		population_size=atoi(argv[4]);;
		max_mutation_step=atof(argv[5]);
		num_crossovers=atoi(argv[6]);
		mutation_prob=atof(argv[7]);
	}
	else {
		printf("USAGE: maxdist <vertices> <file>\n       ");
		printf("maxdist <vertices> <file> <iterations> <population_size> ");
		printf("<max_mutation_step> <num_crossovers> <mutation_prob>\n");
		exit(1);
	}

	// Seed random generator
	srand(time(NULL));

	// Read input file
	polygon = readPolygon(argv[2]);

	// Read number of points
	points = atoi(argv[1]);

	gettimeofday(&start, NULL);

	// Initial population
	population = (Population*) malloc(sizeof(Population));
	population->individuals = (Individual*) malloc(population_size * sizeof(Individual));
	init_population(population, population_size, polygon, points);

	// Do iterations
	for (i = 0; i < iterations; i++) {
		// Select parents for crossover
		population->individuals = (Individual*) realloc(population->individuals,
				(population_size + num_crossovers * 2) * sizeof(Individual));

		parent_collection = select_individuals(polygon, population, population_size, (num_crossovers * 2));
		for (c = 0; c < num_crossovers * 2; c += 2) {
			// Cross-over
			cross_over(parent_collection[c], parent_collection[c+1],
					&population->individuals[population_size + c],
					&population->individuals[population_size + c + 1]);
		}
		free(parent_collection);

		// Mutate children
		for (j = population_size; j < (population_size + num_crossovers * 2); j++) {
			r = ((float) rand() / (float)(RAND_MAX));
			if (r <= mutation_prob) {
				mutate(polygon, &population->individuals[j], max_mutation_step);
			}
		}

		// Calculate fitness of children
		for (j = population_size; j < (population_size + num_crossovers * 2); j++) {
			population->individuals[j].fitness = fitness(&population->individuals[j]);
		}

		// Survival of the fittest
		selection = select_individuals(polygon, population, (population_size+num_crossovers*2), population_size);

		population_buffer = (Population*) malloc(sizeof(Population));
		population_buffer->individuals = (Individual*) malloc(population_size * sizeof(Individual));
		for (j = 0; j < population_size; j++) {
			population_buffer->individuals[j] = *(selection[j]);
			population_buffer->individuals[j].points = (Point*) malloc(points * sizeof(Point));
			for(c=0; c<points; c++) {
				population_buffer->individuals[j].points[c] = selection[j]->points[c];
			}
		}

		free(selection);
		for(j=0; j<population_size+num_crossovers*2; j++) {
			free(population->individuals[j].points);
		}
		free(population->individuals);
		free(population);
		population = population_buffer;
		population_buffer = NULL;

		if(i%100==0){
			total_fitness=0;
			best_fitness = population->individuals[0].fitness;
			for(j=0; j<population_size; j++) {
				total_fitness += population->individuals[j].fitness;
				buffer = population->individuals[j].fitness;
				if (buffer > best_fitness) {
					best_fitness = buffer;
				}
			}
			total_fitness=total_fitness/population_size;
			printf("average fitness:%i,%f\n", i, total_fitness);
			printf("best fitness:%i,%f\n", i, best_fitness);			
		}
	}

	// Select best individual
	// Calculate fitness of individual
	best_fitness = population->individuals[0].fitness;
	best_i = 0;
	for (i = 1; i < population_size; i++) {
		buffer = population->individuals[i].fitness;
		if (buffer > best_fitness) {
			best_fitness = buffer;
			best_i = i;
		}
	}

	// Print result
	printResult(&(population->individuals[best_i]), best_fitness);

	gettimeofday(&end, NULL);
	time_elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.00;
	printf("\nElapsed time: %fs\n", time_elapsed);

	// Clean up
	free(polygon->points);
	free(polygon);
	for (i = 0; i < population_size; i++) {
		free(population->individuals[i].points);
	}
	free(population->individuals);
	free(population);

	return 0;
}
