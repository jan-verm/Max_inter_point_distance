/*
 * maxdist.c
 *
 *  Created on: 13-okt.-2014
 *      Author: Jan Vermeulen
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "geometry.h"
#include "io.h"
#include "genetic.h"
#include "mpitools.h"
#include <mpi.h>

int main(int argc, char* argv[]) {
	Polygon* polygon;
	Population *population, *population_buffer;
	int points, i, j, c, best_i, num_tasks, task_id, a, individuals_to_send, recv_index, tag;
	float best_fitness, buffer, r, *flat1, *flat2;
	Individual **parent_collection, **selection;
	Maxlock_struct *maxlock_struct1, *maxlock_struct2;
	double t1, t2;
	int init_stat;

	int iterations, synchs, population_size, num_crossovers;
	float max_mutation_step, mutation_prob;

	if (argc == 3) {
		// Set default args
		iterations=50000;
		synchs=40;
		population_size=240;
		max_mutation_step=0.9;
		num_crossovers=56;
		mutation_prob=0.5;
	}
	else if (argc == 9) {
		iterations=atoi(argv[3]);;
		synchs=atoi(argv[4]);
		population_size=atoi(argv[5]);;
		max_mutation_step=atof(argv[6]);
		num_crossovers=atoi(argv[7]);
		mutation_prob=atof(argv[8]);
	}
	else {
		printf("USAGE: maxdist <vertices> <file>\n       ");
		printf("maxdist <vertices> <file> <iterations> <synchs> <population_size> ");
		printf("<max_mutation_step> <num_crossovers> <mutation_prob>\n");
		exit(1);
	}

	// Init MPI
	init_stat = MPI_Init(&argc, &argv);
	sleep(10);
	if(init_stat != MPI_SUCCESS){
		exit(1);
	}

	MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&task_id);

	individuals_to_send = population_size / num_tasks;

	// Seed random generator
	srand(time(NULL)+task_id);

	// Read input file
	polygon = readPolygon(argv[2]);

	// Read number of points
	points = atoi(argv[1]);

	// Initial population
	population = (Population*) malloc(sizeof(Population));
	population->individuals = (Individual*) malloc(population_size * sizeof(Individual));
	init_population(population, population_size, polygon, points);

	for(a=0; a<synchs; a++){
		// Do iterations
		for (i = 0; i < iterations; i++) {
			// Select parents for crossover
			population->individuals = (Individual*) realloc(population->individuals,
					(population_size + num_crossovers * 2) * sizeof(Individual));
			parent_collection = select_individuals(polygon, population, population_size, (num_crossovers * 2));

			// Cross-over
			for (c = 0; c < num_crossovers * 2; c += 2) {
				cross_over(parent_collection[c], parent_collection[c+1],
						&population->individuals[population_size + c],
						&population->individuals[population_size + c + 1]);
			}

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
				population_buffer->individuals[j].size = selection[j]->size;
				population_buffer->individuals[j].fitness = selection[j]->fitness;
				
				population_buffer->individuals[j].points = (Point*) malloc(points * sizeof(Point));
				for(c=0; c<points; c++) {
					population_buffer->individuals[j].points[c] = selection[j]->points[c];
				}
			}

			// Clean up
			free(parent_collection);
			free(selection);
			for(j=0; j<population_size+num_crossovers*2; j++) {
				free(population->individuals[j].points);
			}
			free(population->individuals);
			free(population);

			// Initialize new population for next iteration
			population = population_buffer;
			population_buffer = NULL;
		}

		// Synchronize
		flat1 = flatten_population(points, population, population_size);
		flat2 = (float*) malloc(population_size*points*2*sizeof(float));
		MPI_Alltoall(flat1, (points*2*population_size)/num_tasks, MPI_FLOAT, flat2, 
			(points*2*population_size)/num_tasks, MPI_FLOAT, MPI_COMM_WORLD);
		unflatten_population(flat2, points, population, population_size);
		free(flat1);
		free(flat2);
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

	maxlock_struct2 = (Maxlock_struct*) malloc(sizeof(Maxlock_struct));
	maxlock_struct1 = (Maxlock_struct*) malloc(sizeof(Maxlock_struct));
	maxlock_struct1->value = best_fitness;
	maxlock_struct1->rank = task_id;

	MPI_Allreduce(maxlock_struct1, maxlock_struct2, 1, MPI_FLOAT_INT, MPI_MAXLOC, MPI_COMM_WORLD);

	// Print result
	if(task_id == maxlock_struct2->rank) {
		printResult(&(population->individuals[best_i]), best_fitness);
		printf("num_tasks: %i\n", num_tasks);
	}

	// Clean up
	free(polygon->points);
	free(polygon);
	for (i = 0; i < population_size; i++) {
		free(population->individuals[i].points);
	}
	free(population->individuals);
	free(population);
	free(maxlock_struct1);
	free(maxlock_struct2);

	MPI_Finalize();

	return 0;
}
