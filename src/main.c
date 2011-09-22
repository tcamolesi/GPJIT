#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>
#include <math.h>

#include "element.h"
#include "assembler.h"
#include "ettree.h"
#include "ga.h"

typedef struct Plotter {
    BITMAP *back;
} Plotter;

void initPlotter(Plotter *p);
void clearPlotter(Plotter p);
void destroyPlotter(Plotter p);
void plot(Plotter p, JITFunction f, float scalex, float scaley, int col);
void flipPlotter(Plotter p);

inline float myf(float *x) {
    float y = *x;
    return expf(-y*y);
}

float evalFit(JITFunction f) {
    float dif = 0;
    float inp[1];

    for( inp[0] = -5.0f; inp[0] < 5.0f; inp[0] += 0.011f ) {
        float delta = fabs((f(inp) - myf(inp)));
        dif += delta;
    }

    return -dif;
}


int main()
{
    Plotter p;
    srand(time(0));
    initPlotter(&p);
    int i;
    float maxfit = -99999999.0f;
    int indmaxfit = 0;
    RandomElementParams par;
    getRandomElementParams(&par);
    par.variablesRange[0] = 0;
    par.variablesRange[1] = 1;
    par.constantRange[0] = -1;
    par.constantRange[1] = 1;
    setRandomElementParams(&par);

    GeneticAlgorithm ga = createGA(2000);

    FILE *fp = fopen("outx.txt", "w");
    FILE *fp2 = fopen("outx2.txt", "w");

    i = 0;
    while( 1 ) {
        i++;
        runAlgorithm(ga, 100);
        fprintf(fp, "%d\t%.3f\n", i, -getAvgFitness(ga));
        fflush(fp);
        fprintf(fp2, "%d\t%.3f\n", i, -getBestFitness(ga));
        fflush(fp2);
        if( getBestFitness(ga) > maxfit ) {
            indmaxfit = i;
            maxfit = getBestFitness(ga);
            printf("\nNew Best Fitness: fit[%d] = %f\n", i, maxfit );
            printTreeExpression(getBestTree(ga));
            printf("\n");

            clearPlotter(p);
            plot(p, myf, 0.015, 100, makecol(100, 180, 100) );
            plot(p, getBestFunction(ga), 0.015, 100, makecol(180, 100, 100) );
            flipPlotter(p);
        }

        if( i - indmaxfit > 100 ) {
            indmaxfit = i;
            genocideGA(ga);
        }
    }
    return 0;
}
END_OF_MAIN();



void initPlotter(Plotter *p) {
    allegro_init();
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);

    p->back = create_bitmap(640, 480);
}

void clearPlotter(Plotter p) {
    clear(p.back);
    hline(p.back, 0, 240, 640, makecol(0,0,128));
    vline(p.back, 320, 0, 480, makecol(0,0,128));
}

void plot(Plotter p, JITFunction f, float scalex, float scaley, int col) {
    int i;
    float lastres = 0;
    for(i = 0; i < 640; i++ ) {
        float arg = (i - 320)* scalex;
        float curres = f(&arg)*scaley;
          line(p.back, i - 1, 240 - lastres, i, 240 - curres, col);
          lastres = curres;
    }
}

void flipPlotter(Plotter p) {
    blit(p.back, screen, 0, 0, 0, 0, 640, 480);
}

void destroyPlotter(Plotter p) {
    destroy_bitmap(p.back);
}
