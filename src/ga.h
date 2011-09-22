#ifndef GA_H_INCLUDED
#define GA_H_INCLUDED

#include "ettree.h"
#include "assembler.h"

typedef enum {
    GA_SELECT_BEST
} SelectionMethod;

typedef struct GeneticAlgorithm *GeneticAlgorithm;

GeneticAlgorithm createGA(int popsize);
GeneticAlgorithm setGAParameters(float mutRate, SelectionMethod sm);
float getBestFitness(GeneticAlgorithm ga);
float getAvgFitness(GeneticAlgorithm ga);
ETTree getBestTree(GeneticAlgorithm ga);
JITFunction getBestFunction(GeneticAlgorithm ga);
void runAlgorithm(GeneticAlgorithm ga, int stepCount);
void printAll(GeneticAlgorithm ga);
void genocideGA(GeneticAlgorithm ga);
#endif // GA_H_INCLUDED
