#include "rect.h"

#define b 100
#define B 4096


typedef struct{
	rect *r;
	int child;
}nodeVal;

typedef struct{
	int size;
	int address;
	nodeVal *values[2*b+1];
	rect *MBR;
	int leaf;
}node;


node *makeNode(int leaf);
node *readNode(int address);
void writeNode(node *n);

//Libera el nodo de memoria principal
void freeNode(node *n);

//Libera el node de memoria principal y secundaria
void destroyNode(node *n);


nodeVal *makeNodeVal(rect *r, int child);
void freeNodeVal(nodeVal *nv);

int getReads();
int getWrites();
