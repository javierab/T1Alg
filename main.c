#include <stdio.h>
#include <stdlib.h>

#include "tarea1.h"
#include "RTree.h"

int main(){
	nodeVal *val = new(nodeVal);
	val->r = makeRect(1,1,3,2);
	val->child = -1;

	nodeVal *val2 = new(nodeVal);
	val2->r = makeRect(2,2,4,3);
	val2->child = -1;

	nodeVal *val3 = new(nodeVal);
	val3->r = makeRect(10,1,12,2);
	val3->child = -1;

	nodeVal *val4 = new(nodeVal);
	val4->r = makeRect(3,1,5,2);
	val4->child = -1;

	RTree *tree = makeTree();
	insert(tree, val);
	insert(tree, val2);
	insert(tree, val3);
    insert(tree, val4);
    
    int k;
    rect *r = makeRect(4.5,1,20,20);
    twoInts **resp;
    k = search(r, tree, resp);
    fprintf(stderr, "%d\n", k);

	freeRTree(tree);
}
