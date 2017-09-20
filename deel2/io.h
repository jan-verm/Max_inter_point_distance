/*
 * io.h
 *
 *  Created on: 13-okt.-2014
 *      Author: Jan Vermeulen
 */

#ifndef IO_H_
#define IO_H_

#include "geometry.h"
#include "genetic.h"

Polygon* readPolygon(char* filename);

void printResult(Individual* i, float fitness);

#endif
