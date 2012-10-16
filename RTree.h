#include "tarea1.h"
#include "node.h"




typedef struct{
	int int1;
	int int2;
}twoInts;


typedef struct{
	node *root;
}RTree;


typedef struct{
	node *node1; 
	node *node2;
}insertVal;


RTree *makeTree();
RTree *readTree();
void freeRTree(RTree *t);
void insert(RTree *t, nodeVal *v);
void freeNode(node *n);
int search(rect *r, RTree *t, int **resp);
void setInsertMethod(int i);
void writeRTree(RTree *t);

