/*
 * geometry.c
 *
 *  Created on: 13-okt.-2014
 *      Author: jan
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "geometry.h"

float distance(Point from, Point to) {
	return sqrtf( powf((from.x-to.x),2) + powf((from.y-to.y),2) );
}

int inside(Polygon* polygon, Point point) {
	int i, j, lower_than_zero, higher_than_zero;
	float a;
	Polygon* copy;
	copy = copyPolygon(polygon);

	// point is new origin
	for(i=0; i<copy->edges; i++){
		copy->points[i].x -= point.x;
		copy->points[i].y -= point.y;
	}

	lower_than_zero = 1;
	higher_than_zero = 1;
	for(i=0; i<copy->edges; i++) {
		j=(i+1)%copy->edges;
		a = copy->points[j].x*copy->points[i].y - copy->points[i].x*copy->points[j].y;
		if (a>0) {
			lower_than_zero = 0;
		}
		if (a<0) {
			higher_than_zero = 0;
		}
	}

	free(copy->points);
	free(copy);

	if (lower_than_zero || higher_than_zero) {
		return 1;
	}
	else {
		return 0;
	}
}


float triangle_surface(Point* a, Point* b, Point* c) {
	return abs((a->x*(b->y-c->y)+b->x*(c->y-a->y)+c->x*(a->y-b->y))/2);
}

Polygon* copyPolygon(Polygon* src) {
	int i;
	Polygon* dest;

	dest = (Polygon*) malloc(sizeof(Polygon));
	dest->points = (Point*) malloc(src->edges * sizeof(Point));

	dest->edges = src->edges;
	for(i=0; i<src->edges; i++) {
		dest->points[i].x = src->points[i].x;
		dest->points[i].y = src->points[i].y;
	}

	return dest;
}

void printPolygon(Polygon* p) {
	int i;
	printf("edges: %i\n", p->edges);
	for(i=0; i<p->edges; i++){
		printf("%f %f\n", p->points[i].x, p->points[i].y);
	}
}
