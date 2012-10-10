#include "rect.h"
#include "tarea1.h"

#define b 1
#define B 4096

typedef struct{
	rect *r;
	int child;
}nodeVal;

typedef struct{
	int int1;
	int int2;
}twoInts;


typedef struct{
	int size;
	int address;
	nodeVal *values[2*b+1];
	rect *MBR;
	int leaf;
}node;

typedef struct{
	node *root;
}RTree;


typedef struct{
	node *node1; //nombre de nodo
	node *node2;
}insertVal;


RTree *makeTree();
void insert(RTree *t, nodeVal *v);
void freeNode(node *n);
void freeNodeVal(nodeVal *nv);
int search(rect *r, RTree *t, twoInts **resp);

