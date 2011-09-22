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
#include <memory.h>
#include "element.h"

#define _OP(x,y,z) z,
char *_opstring[] = {
#include "optypes.inc"
};
#undef _OP

#define _IFUNC(x,y,z) { #y, z },
IFuncInfo _ifuncs[] = {
#include "ifuncs.inc"
};
#undef _IFUNC

RandomElementParams _randomizerParams = {
    { 400, 150, 350, 100},
    { 300, 250, 300, 100 },
    { 300, 300, 250, 75, 75 },
    { -1000.0f, 1000.0f },
    { 0, 2 }
};

void setRandomElementParams( const RandomElementParams* params ) {
    memcpy( &_randomizerParams, params, sizeof( RandomElementParams ) );
}

void getRandomElementParams( RandomElementParams* params ) {
    memcpy( params, &_randomizerParams, sizeof( RandomElementParams ) );
}

Element getRandomElement2(Element base) {
    Element e;

    e.type = base.type;
    int i, tmp;
    if( base.type == ELEM_OPERATOR ) {
            e.op = 0;
            tmp = rand() % 1000;
            for ( i = 0; i < OPTYPE_MAX; i++ ) {
                    tmp -= _randomizerParams.probOperators[i];
                    if ( tmp <= 0 ) {
                            e.op = i;
                            break;
                        }
                }
    } else if( base.type == ELEM_CONSTANT || base.type == ELEM_VARIABLE ) {
                if( rand() % 2 == 0 ) {
                    e.type = ELEM_CONSTANT;
                    float cr0 = _randomizerParams.constantRange[0];
                    float cr1 = _randomizerParams.constantRange[1];
                    e.f = rand() / ( 1.0f * RAND_MAX ) * ( cr1 - cr0 ) + cr0;
                    } else {
                        e.type = ELEM_VARIABLE;
                        int v0 = _randomizerParams.variablesRange[0];
                        int v1 = _randomizerParams.variablesRange[1];
                        e.varnum = rand() % ( v1 - v0 ) + v0;
                    }
    } else if( base.type == ELEM_IFUNC ) {
            e.func = base.func;
    }

    return e;
}

Element getRandomElement() {
    Element e;
    int i;

    e.type = 0;
    int tmp = rand() % 1000;
    for ( i = 0; i < ELEMTYPE_MAX; i++ ) {
            tmp -= _randomizerParams.probType[i];
            if ( tmp <= 0 ) {
                    e.type = i;
                    break;
                }
        }

    switch ( e.type ) {
        case ELEM_OPERATOR:
            e.op = 0;
            tmp = rand() % 1000;
            for ( i = 0; i < OPTYPE_MAX; i++ ) {
                    tmp -= _randomizerParams.probOperators[i];
                    if ( tmp <= 0 ) {
                            e.op = i;
                            break;
                        }
                }
            break;

        case ELEM_CONSTANT:
            {
                float cr0 = _randomizerParams.constantRange[0];
                float cr1 = _randomizerParams.constantRange[1];
                e.f = rand() / ( 1.0f * RAND_MAX ) * ( cr1 - cr0 ) + cr0;
            }
            break;

        case ELEM_VARIABLE:
            {
                int v0 = _randomizerParams.variablesRange[0];
                int v1 = _randomizerParams.variablesRange[1];
                e.varnum = rand() % ( v1 - v0 ) + v0;
            }
            break;

        case ELEM_IFUNC:
            e.func = 0;
            tmp = rand() % 1000;
            for ( i = 0; i < IFUNCTYPE_MAX; i++ ) {
                    tmp -= _randomizerParams.probIFunc[i];
                    if ( tmp <= 0 ) {
                            e.func = i;
                            break;
                        }
                }
            break;

        default:
            break;
        }

    return e;
}

void printElement( Element e ) {
    switch ( e.type ) {
        case ELEM_OPERATOR:
            printf( "%s", _opstring[e.op] );
            break;

        case ELEM_VARIABLE:
            printf( "$%d", e.varnum );
            break;

        case ELEM_CONSTANT:
            printf( "%.2f", e.f );
            break;

        case ELEM_IFUNC:
            printf( "%s", _ifuncs[e.func].name );
            break;

        default:
            break;
        }
}

IFuncInfo getIFuncInfo(int id) {
    return _ifuncs[id];
}
