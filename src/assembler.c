//-----------------------------------------------------------------------------
//Nome: Tiago Camolesi Flora
//N USP: 5655201
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                                Headers
//-----------------------------------------------------------------------------
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include "assembler.h"
#include "element.h"

//-----------------------------------------------------------------------------
//                             Implementation
//-----------------------------------------------------------------------------

#define _ET(x,y) void assemble_##y (Assembler, Element);
#include "elemtypes.inc"
#undef _ET

#define _OP(x,y,z) void assemble_op_##y (Assembler, Element);
#include "optypes.inc"
#undef _OP

#define _IFUNC(x,y,z) void assemble_ifunc_##y (Assembler, Element);
#include "ifuncs.inc"
#undef _IFUNC

struct Assembler {
    char *base, *pos, *data, *end;
    int id;
    AssemblerFactory origin;
};

struct AssemblerFactory {
    int maxProgramSize;
    int maxProgramCount;
    void *dataPool;
    int *availableSlots;
    int curSlot;
};

typedef void (*AsmFunc)(Assembler a, Element e);

#define _ET(x,y) assemble_##y,
AsmFunc _asmFuncs[] = {
#include "elemtypes.inc"
};
#undef _ET

#define _OP(x,y,z) assemble_op_##y,
AsmFunc _asmOps[] = {
#include "optypes.inc"
};
#undef _OP

#define _IFUNC(x,y,z) assemble_ifunc_##y,
AsmFunc _asmIFuncs[] = {
#include "ifuncs.inc"
};
#undef _IFUNC

void printCode(Assembler a);

//------------------------------------------------------------------------------
//AssemblerFactory: Implementation
//------------------------------------------------------------------------------

AssemblerFactory createAssemblerFactory(int maxProgramSize,
    int maxProgramCount) {
        AssemblerFactory factory = malloc(sizeof(struct AssemblerFactory));

        factory->curSlot = 0;
        factory->maxProgramCount = maxProgramCount;
        factory->maxProgramSize = maxProgramSize;

        factory->availableSlots = calloc(sizeof(int), maxProgramCount);
        factory->dataPool = mmap(NULL, maxProgramSize * maxProgramCount,
         PROT_EXEC | PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        return factory;
}

void destroyAssemblerFactory(AssemblerFactory factory) {
    if( factory->availableSlots != 0 ) {
    free(factory->availableSlots);
    }

    if(factory->dataPool != 0) {
    munmap(factory->dataPool, factory->maxProgramCount * factory->maxProgramSize );
    }

    free(factory);
}

void getAvailableSlot(AssemblerFactory factory, Assembler a) {
    int i;
    int ind = factory->curSlot;

    a->id = -1;

    i = ind;
    do {
        if( ++i >= factory->maxProgramCount ) {
            i = 0;
        }
        if( factory->availableSlots[i] == 0 ) {
            factory->curSlot = i;
            factory->availableSlots[i] = 1;

            a->id = i;
            a->base = (void*) ( factory->dataPool + i * factory->maxProgramSize);

            return;
        }
    } while( i != ind );
}

void freeSlot(AssemblerFactory factory, int id) {
    factory->availableSlots[id] = 0;
}

//------------------------------------------------------------------------------
//Assembler: Creation/Destruction
//------------------------------------------------------------------------------

Assembler createAssembler(AssemblerFactory factory) {
    Assembler a = malloc(sizeof(struct Assembler));

    getAvailableSlot(factory, a);

    if( a->id == -1 ) {
        free(a);
        return 0;
    }

    a->origin = factory;
    a->end = a->base + a->origin->maxProgramSize - 1;
    a->data = a->end;
    a->pos = a->base;

    int i = 0;
    for( i = 0; i < a->origin->maxProgramSize; i++ ) {
        a->base[i] = 0;
    }

    return a;
}

void destroyAssembler(Assembler a) {
    freeSlot(a->origin, a->id);
    free(a);
}

JITFunction getJitFunction(Assembler a) {
    return (JITFunction) a->base;
}

void assembleTree(Assembler a, ETTree t) {
    int i;
    int bc = getBranchCount(t);
    if( bc > 0 ) {
        for(i = 0; i < bc; i++ ) {
            assembleTree(a, getBranch(t, i));
        }
    }
    assembleElement(a, getElement(t));
}

//------------------------------------------------------------------------------
//Assembler: VTable
//------------------------------------------------------------------------------

void assembleElement(Assembler a, Element e) {
    _asmFuncs[e.type](a, e);
}

void assemble_operator(Assembler a, Element e) {
    _asmOps[e.op](a,e);
}

void assemble_ifunc(Assembler a, Element e) {
    _asmIFuncs[e.func](a,e);
}

//------------------------------------------------------------------------------
//Assembler: Tools
//------------------------------------------------------------------------------

inline void insertCode(Assembler a, void *code, int size ) {
    memcpy(a->pos, code, size);
    a->pos += size;
}

inline void insertCodeByte(Assembler a, char code) {
    *(a->pos) = code;
    a->pos++;
}

inline char* insertData(Assembler a, void *code, int size ) {
    a->data -= size;
    memcpy(a->data, code, size);

    return a->data;
}

void printCode(Assembler a) {
    int i;
    for(i = 0; i < a->origin->maxProgramSize; i++ ) {
        if( i > 0 && i % 16 == 0 ) {
            printf( "\n" );
        }
        printf("%.2x ", ((int) a->base[i]) & 0xFF );
    }

    printf("\n-----------------------------------\n");
}

//------------------------------------------------------------------------------
//Assembler: Begin/End
//------------------------------------------------------------------------------

void beginAssembly(Assembler a) {
    a->pos = a->base;
    a->data = a->end;

    //#PUSH EBP
    //#MOV EBP, ESP
    //PUSH EBX
    //PUSH EDI
    //MOV EBX, a->data
    //#MOV EDI, DWORD PTR [EBP + 0x08]
    //MOV EDI, DWORD PTR [ESP + 0x0C]

    /*insertCode(a, "\x55\x89\xE5\x53\x57\xBB", 6);
    insertCode(a, &a->data, 4 );
    insertCode(a, "\x8B\x7D\x08", 3);*/

    insertCode(a, "\x53\x57\xBB", 3);
    insertCode(a, &a->data, 4 );
    insertCode(a, "\x8B\x7C\x24\x0C", 4);
}

void finishAssembly(Assembler a) {
    //POP EDI
    //POP EBX
    //#LEAVE
    //RET

    //insertCode(a, "\x5F\x5B\xC9\xC3", 4 );
    insertCode(a, "\x5F\x5B\xC3", 3 );
}

//------------------------------------------------------------------------------
//Assembler: Operations
//------------------------------------------------------------------------------

void assemble_variable(Assembler a, Element e) {
    //FLD DWORD PTR [EDI + varnum * 4]

    if( e.varnum < 32 ) { //Byte displacement
        insertCode(a, "\xD9\x47", 2);
        insertCodeByte(a, (e.varnum * 4) & 0xFF );
    } else { //Dword displacement
        int tmp = e.varnum * 4;
        insertCode(a, "\xD9\x87", 2);
        insertCode(a, &tmp, 4 );
    }
}

void assemble_constant(Assembler a, Element e) {
    //FLD dword [EBX + delta]

    char *ptr = insertData(a, &e.f, 4 );
    int delta = ( (int) ptr ) - ((int) a->end );

    if ( delta > -127 ) { //Byte displacement
        insertCode(a, "\xD9\x43", 2);
        insertCodeByte(a, delta & 0xFF );
    } else { //Dword displacement
        insertCode(a, "\xD9\x83", 2);
        insertCode(a, &delta, 4 );
    }
}

void assemble_op_add(Assembler a, Element e) {
    //ADDP st1

    insertCode(a, "\xDE\xC1", 2);
}

void assemble_op_mul(Assembler a, Element e) {
    //MULP st1

    insertCode(a, "\xDE\xC9", 2);
}

void assemble_op_div(Assembler a, Element e) {
    //DIVP st1

    insertCode(a, "\xDE\xF9", 2);
}

void assemble_op_sub(Assembler a, Element e) {
    //SUBP st1

    insertCode(a, "\xDE\xE9", 2);
}

void assemble_ifunc_max(Assembler a, Element e) {
    //FUCOMI st1
    //FCMOVB st1
    //FFREE st1

    insertCode(a, "\xDB\xE9\xDA\xC1\xDD\xD9", 6);
}

void assemble_ifunc_min(Assembler a, Element e) {
    //FUCOMI st1
    //FCMOVNB st1
    //FSTP st1

    insertCode(a, "\xDB\xE9\xDB\xC1\xDD\xD9", 6);
}

void assemble_ifunc_sin(Assembler a, Element e) {
    //FSIN

    insertCode(a, "\xD9\xFE", 2);
}

void assemble_ifunc_cos(Assembler a, Element e) {
    //FCOS

    insertCode(a, "\xD9\xFF", 2);
}

void assemble_ifunc_abs(Assembler a, Element e) {
    //FABS

    insertCode(a, "\xD9\xE1", 2);
}
