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
#include "ettree.h"
#include "element.h"

#define INIT_MAX_BRANCHES 4

struct ETTree {
        Element e;
        int maxBranches;
        int branchCount;
        ETTree parent;
        ETTree *branches;
    };

Element getFirstElement(ETTree tree ) {
    if( tree->branchCount == 0 ) {
        return tree->e;
    } else {
        return getFirstElement(tree->branches[0]);
    }
}

void pruneTree(ETTree tree, int maxlevel) {
    int i;
    if( tree->branchCount > 0) {
        if ( maxlevel > 1 ) {
            for ( i = 0; i < tree->branchCount; i++ ) {
                pruneTree(tree->branches[i], maxlevel - 1);
            }
        } else {
            tree->e = getFirstElement(tree->branches[0]);
            for( i = 0; i < tree->branchCount; i++ ) {
                //destroyTree(tree->branches[i]);
                removeSubTree(tree, i);
            }
            }
        }
    }

ETTree createTree() {
    ETTree e = malloc( sizeof( struct ETTree ) );

    e->maxBranches = INIT_MAX_BRANCHES;
    e->branchCount = 0;
    e->branches = malloc( sizeof( ETTree ) * e->maxBranches );
    e->parent = 0;

    return e;
}

ETTree getParentTree(ETTree tree) {
    return tree->parent;
}

void swapSubTree(ETTree tree, ETTree subtree, ETTree newSubTree) {
    int i;

    for( i = 0; i < tree->branchCount; i++ ) {
        if( tree->branches[i] == subtree ) {
            newSubTree->parent = tree;
            tree->branches[i] = newSubTree;
            return;
        }
    }
}

void destroyTree( ETTree tree ) {
    int i;
    for ( i = 0; i < tree->branchCount; i++ ) {
            destroyTree( tree->branches[i] );
        }

    free( tree->branches );
    free( tree );
}

ETTree cloneTree( ETTree tree ) {
    int i;
    ETTree result = createTree();

    setElement( result, tree->e );

    for ( i = 0; i < tree->branchCount; i++ ) {
            addSubTree( result, cloneTree( tree->branches[i] ) );
        }

    return result;
}

ETTree getBranch( ETTree tree, int n ) {
    return tree->branches[n];
}

void addSubTree( ETTree tree, ETTree subtree ) {
    if ( tree->branchCount >= tree->maxBranches ) {
            int size = sizeof( ETTree ) * tree->maxBranches * 2;
            tree->branches = realloc( tree->branches, size );
            tree->maxBranches *= 2;
        }

    tree->branches[tree->branchCount++] = subtree;
    subtree->parent = tree;
}

void removeSubTree( ETTree tree, int n ) {
    destroyTree( tree->branches[n] );
    detachSubTree( tree, n );
}

void detachSubTree( ETTree tree, int n ) {
    tree->branchCount--;
    tree->branches[n]->parent = 0;
    while ( n < tree->branchCount ) {
            tree->branches[n] = tree->branches[n+1];
            n++;
        }
}

void setElement( ETTree tree, Element el ) {
    tree->e.type = el.type;
    tree->e.op = el.op;
}

Element getElement( ETTree tree ) {
    return tree->e;
}

int getBranchCount( ETTree tree ) {
    return tree->branchCount;
}

void printTree( ETTree tree, int indent ) {
    int i;
    for ( i = 0; i < indent; i++ ) {
            printf( "  " );
        }
    printElement( getElement( tree ) );
    printf( "\n" );
    for ( i = 0; i < getBranchCount( tree ); i++ ) {
            printTree( getBranch( tree, i ), indent + 1 );
        }
}

void printTreeExpression( ETTree tree ) {
    int i;
    switch ( tree->e.type ) {
        case ELEM_OPERATOR:
            printf( "(" );
            printTreeExpression( tree->branches[0] );
            printf( " " );
            printElement( tree->e );
            printf( " " );
            printTreeExpression( tree->branches[1] );
            printf( ")" );
            break;

        case ELEM_IFUNC:
            printElement( tree->e );
            if( tree->branchCount > 0 ) {
                printf( "[" );
                printTreeExpression( tree->branches[0] );
                for( i = 1; i < tree->branchCount; i++ ) {
                    printf( ", " );
                    printTreeExpression( tree->branches[i] );
                }
                printf( "]" );
            }
            break;

        default:
            printElement( tree->e );
        }
}


ETTree genRandomTree( int maxDepth ) {
    Element e;
    ETTree tree = createTree();

    do {
            e = getRandomElement();
        } while (( e.type == ELEM_OPERATOR || e.type == ELEM_IFUNC )
             && maxDepth <= 0 );

    setElement( tree, e );

    if ( e.type == ELEM_OPERATOR ) {
            addSubTree( tree, genRandomTree( maxDepth - 1 ) );
            addSubTree( tree, genRandomTree( maxDepth - 1 ) );
        } else if ( e.type == ELEM_IFUNC ) {
            int i, nargs;
            nargs = getIFuncInfo( e.func ).nargs;
            for ( i = 0; i < nargs; i++ ) {
                    addSubTree( tree, genRandomTree( maxDepth - 1 ) );
                }
        }

    return tree;
}
