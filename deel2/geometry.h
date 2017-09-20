/*
 * datastructures.h
 *
 *  Created on: 13-okt.-2014
 *      Author: Jan Vermeulen
 */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

typedef struct {
	float x;
	float y;
} Point;

typedef struct {
	int edges;
	Point* points;
} Polygon;

float distance(Point from, Point to);

Polygon* copyPolygon(Polygon* src);

void printPolygon(Polygon* p);

int inside(Polygon* polygon, Point point);

float triangle_surface(Point* p1, Point* p2, Point* p3);

#endif
