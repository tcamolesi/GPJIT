//-----------------------------------------------------------------------------
//Nome: Tiago Camolesi Flora
//N USP: 5655201
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                                Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                             Implementation
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ga.h"
#include "assembler.h"
#include "ettree.h"
#include "element.h"

#define MAX_TREE_DEPTH 7

typedef struct Individual {
  Assembler a;
  ETTree tree;
  float fitness;
  GeneticAlgorithm owner;
  int changed;
} Individual;

struct GeneticAlgorithm{
  AssemblerFactory fac;
  Individual* pop;
  Individual* best;
  int popsize;
  float mutRate;
};

void printAll(GeneticAlgorithm ga) {
  int i;
  for( i = 0; i < ga->popsize; i++ ) {
    printf("[%d] : \n", i );
    printTreeExpression(ga->pop[i].tree);
    printf("\n----------------------------\n" );
    fflush(stdout);
  }
}

extern float myf(float *x);

float evalFitness(Individual *id) {
  float dif = 0;
  float inp[1];
  JITFunction f = getJitFunction(id->a);

  for( inp[0] = -4.95f; inp[0] < 5.0f; inp[0] += 0.1f ) {
    float delta = fabs((f(inp) - myf(inp)));
    dif += delta;
  }

  return -dif;
}

void updateFitness(GeneticAlgorithm ga) {
  int i;
  ga->best = &ga->pop[0];

  for( i = 0; i < ga->popsize; i++ ) {
    while( ga->pop[i].changed ) {
      beginAssembly(ga->pop[i].a);
      assembleTree(ga->pop[i].a, ga->pop[i].tree );
      finishAssembly(ga->pop[i].a);
      ga->pop[i].fitness = evalFitness(&ga->pop[i]);
      if( !isnan(ga->pop[i].fitness) && !isinf(ga->pop[i].fitness) ) {
        ga->pop[i].changed = 0;
      } else {
        destroyTree(ga->pop[i].tree);
        ga->pop[i].tree = genRandomTree(MAX_TREE_DEPTH);
      }
    }
    if( ga->pop[i].fitness > ga->best->fitness ) {
      ga->best = &ga->pop[i];
    }
  }
}

void crossOver(GeneticAlgorithm ga) {
  ETTree it1, it2;
  int i, j;
  int prob = 300;

  do {
    i = rand() % ga->popsize;
    j = rand() % ga->popsize;
  } while( i == j );

  if ( ga->pop[i].fitness > ga->pop[j].fitness ) {
    int tmp = i;
    i = j;
    j = tmp;
  }


  it1 = ga->pop[i].tree;
  it2 = ga->pop[j].tree;

  while( 1 ) {
    if( rand() % 1000 < prob ) {
      break;
    } else {
      int bc = getBranchCount(it1);
      if( bc == 0 ) {
        break;
      }
      it1 = getBranch(it1, rand() % bc);
      prob += 100;
    }
  }

  prob = 300;
  while( 1 ) {
    if( rand() % 1000 < prob ) {
      break;
    } else {
      int bc = getBranchCount(it2);
      if( bc == 0 ) {
        break;
      }
      it2 = getBranch(it2, rand() % bc );
      prob += 100;
    }
  }

  if( getParentTree(it1) == 0 && getParentTree(it2) != 0 ) {
    ga->pop[i].changed = 1;
    destroyTree(it1);
    ga->pop[i].tree = it1 = genRandomTree(MAX_TREE_DEPTH);
  }

  if( getParentTree(it1) != 0 && getParentTree(it2) != 0 ) {
    ETTree b = cloneTree(it2);
    if( &ga->pop[j] != ga->best ) {
      ETTree a = cloneTree(it1);
      swapSubTree(getParentTree(it2), it2, a);
      destroyTree(it2);
      pruneTree(ga->pop[j].tree, MAX_TREE_DEPTH);
      ga->pop[j].changed = 1;
    }
    swapSubTree(getParentTree(it1), it1, b);
    destroyTree(it1);
    pruneTree(ga->pop[i].tree, MAX_TREE_DEPTH);
    ga->pop[i].changed = 1;
  }

}



void mutate(GeneticAlgorithm ga) {
  ETTree it1;
  int iMutRate = ga->mutRate * 1000;
  int i, j;

  for( i = 0; i < ga->popsize/10; i++ ) {
    do {
      j = rand() % ga->popsize;
    } while (&ga->pop[j] == ga->best);

    it1 = ga->pop[j].tree;

    while( getBranchCount(it1) > 0 ) {
      if ( rand() % 1000 < iMutRate ) {
        Element e = getElement(it1);
        Element enew;

        enew = getRandomElement2(e);
        setElement(it1, enew);
        ga->pop[j].changed = 1;
        break;
      }
      it1 = getBranch(it1, rand() % getBranchCount(it1) );
    }
  }
}

GeneticAlgorithm createGA(int popsize) {
  int i;

  GeneticAlgorithm ga = malloc( sizeof(struct GeneticAlgorithm) );

  ga->mutRate = 0.1f;
  ga->popsize = popsize;
  ga->pop = calloc( popsize, sizeof(struct Individual));
  ga->fac = createAssemblerFactory(1024, popsize);
  ga->best = 0;

  for( i = 0; i < popsize; i++ ) {
    ga->pop[i].a = createAssembler(ga->fac);
    ga->pop[i].owner = ga;
  }

  genocideGA(ga);

  return ga;
}

void genocideGA(GeneticAlgorithm ga) {
  int i;
  for( i = 0; i < ga->popsize; i++ ) {
    if( !ga->best || &ga->pop[i] != ga->best ) {
      ga->pop[i].tree = genRandomTree(MAX_TREE_DEPTH);
      ga->pop[i].changed = 1;
    }
  }
}

float getBestFitness(GeneticAlgorithm ga) {
  return ga->best->fitness;
}

JITFunction getBestFunction(GeneticAlgorithm ga) {
  return getJitFunction(ga->best->a);
}

float getAvgFitness(GeneticAlgorithm ga) {
  float avg = 0;
  int i;
  for( i = 0; i < ga->popsize; i++ ) {
    //printf("[%d]: %f\n", i, ga->pop[i].fitness);
    avg += ga->pop[i].fitness;
  }
  return avg/( (float)  ga->popsize );
}

ETTree getBestTree(GeneticAlgorithm ga) {
  return ga->best->tree;
}

void runAlgorithm(GeneticAlgorithm ga, int stepCount) {
  int i, j;

  for( i = 0; i < stepCount; i++ ) {
    updateFitness(ga);
    for( j = 0; j < ga->popsize/10; j++ ) {
      crossOver(ga);
    }
    mutate(ga);
  }
}
