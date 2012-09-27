#include <stdio.h>
#include <stdlib.h>

#include "tarea1.h"
#include "RTree.h"

int main(){
	nodeVal *val = new(nodeVal);
	val->r = makeRect(1,3,3,1);
	val->child = 3;


	nodeVal *val2 = new(nodeVal);
	val2->r = makeRect(2,2.5,4.5,1.5);
	val2->child = 1;

	nodeVal *val3 = new(nodeVal);
	val3->r = makeRect(4,2,5,1);
	val3->child = 1;

	RTree *tree = makeTree();
	insert(tree, val);
	insert(tree, val2);
	insert(tree, val3);

}
