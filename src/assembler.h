#ifndef ASSEMBLER_H_INCLUDED
#define ASSEMBLER_H_INCLUDED

#include "element.h"
#include "ettree.h"

typedef float (*JITFunction)(float *);

typedef struct Assembler *Assembler;
typedef struct AssemblerFactory *AssemblerFactory;

AssemblerFactory createAssemblerFactory(int maxProgramSize,int maxProgramCount);
void destroyAssemblerFactory(AssemblerFactory factory);

Assembler createAssembler(AssemblerFactory factory);
void beginAssembly(Assembler a);
void assembleElement(Assembler a, Element e);
void finishAssembly(Assembler a);
JITFunction getJitFunction(Assembler a);
void destroyAssembler(Assembler a);
void assembleTree(Assembler a, ETTree t);

#endif // ASSEMBLER_H_INCLUDED
