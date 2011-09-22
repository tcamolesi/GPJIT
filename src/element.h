#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED

#define _ET(x,y) ELEM_##x,
typedef enum {
    #include "elemtypes.inc"
    ELEMTYPE_MAX
} ElemType;
#undef _ET

#define _OP(x,y,z) OP_##x,
typedef enum {
    #include "optypes.inc"
    OPTYPE_MAX
} OpType;
#undef _OP

#define _IFUNC(x,y,z) IFUNC_##x,
typedef enum {
    #include "ifuncs.inc"
    IFUNCTYPE_MAX
} IFuncType;
#undef _IFUNC

typedef struct IFuncInfo {
    char* name;
    int nargs;
} IFuncInfo;


typedef struct Element {
    ElemType type;
    union {
        OpType op;
        IFuncType func;
        float f;
        int varnum;
    };
} Element;

typedef struct RandomElementParams {
    int probType[ELEMTYPE_MAX];
    int probOperators[OPTYPE_MAX];
    int probIFunc[IFUNCTYPE_MAX];
    float constantRange[2];
    int variablesRange[2];
} RandomElementParams;

void printElement(Element e);
void setRandomElementParams(const RandomElementParams* params);
void getRandomElementParams( RandomElementParams* params );
Element getRandomElement();
Element getRandomElement2(Element base);
IFuncInfo getIFuncInfo(int id);

#endif // ELEMENT_H_INCLUDED
