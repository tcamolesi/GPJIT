#ifndef ETTREE_H_INCLUDED
#define ETTREE_H_INCLUDED

#include "element.h"

typedef struct ETTree* ETTree;

ETTree createTree();
void destroyTree(ETTree tree);
ETTree cloneTree(ETTree tree);
ETTree getBranch(ETTree tree, int n);

void pruneTree(ETTree tree, int maxlevel);
void addSubTree(ETTree tree, ETTree subtree);
void swapSubTree(ETTree tree, ETTree subtree, ETTree newSubTree);
ETTree getParentTree(ETTree tree);
void removeSubTree(ETTree tree, int n);
void detachSubTree(ETTree tree, int n);
void setElement(ETTree tree, Element e);
Element getElement(ETTree tree);
void printTree(ETTree tree, int indent);
void printTreeExpression(ETTree tree);

ETTree genRandomTree(int maxDepth);
int getBranchCount(ETTree tree);

#endif // ETTREE_H_INCLUDED
