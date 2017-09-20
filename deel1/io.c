/*
 * io.c
 *
 *  Created on: 13-okt.-2014
 *      Author: jan
 */

#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "geometry.h"

Polygon* readPolygon(char* filename) {
	int i;
	FILE *fp;
	Polygon* polygon;
	polygon = (Polygon*) malloc(sizeof(Polygon));

	fp = fopen(filename,"r");
	if( fp == NULL ) {
		printf("Error opening file.\n");
		exit(1);
	}

	fscanf(fp, "%i\n", &(polygon->edges));
	polygon->points = (Point*) malloc((polygon->edges) * sizeof(Point));
	for(i=0; i<polygon->edges; i++){
		fscanf(fp, "%f %f\n", &(polygon->points[i].x), &(polygon->points[i].y));
	}

	fclose(fp);

	return polygon;
}

void printResult(Individual* individual, float fitness) {
	int i;

	printf("%f\n", fitness);
	for (i = 0; i < individual->size; i++) {
		printf("%f %f\n", individual->points[i].x,
				individual->points[i].y);
	}
}
