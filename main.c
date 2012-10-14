#include <stdio.h>
#include <stdlib.h>

#include "tarea1.h"
#include "RTree.h"

int size_resp = 100;


int main(){
	nodeVal *val = new(nodeVal);
	val->r = makeRect(1,1,3,2);
	val->child = -1;

	nodeVal *val2 = new(nodeVal);
	val2->r = makeRect(2,2,4,3);
	val2->child = -1;

	nodeVal *val3 = new(nodeVal);
	val3->r = makeRect(40,30,50,40);
	val3->child = -1;

	nodeVal *val4 = new(nodeVal);
	val4->r = makeRect(35,40,60,45);
	val4->child = -1;

	nodeVal *val5 = new(nodeVal);
	val5->r = makeRect(10,12,13,15);
	val5->child = -1;
    
	nodeVal *val6 = new(nodeVal);
	val6->r = makeRect(12,15,20,20);
	val6->child = -1;
    
    nodeVal *val7 = new(nodeVal);
	val7->r = makeRect(3,4,5,6);
	val7->child = -1;
        
        
        
    nodeVal *val0 = new(nodeVal);
	val0->r = makeRect(1,2,4,5);
	val0->child = -1;
    
    nodeVal *val8 = new(nodeVal);
	val8->r = makeRect(1,1,3,2);
	val8->child = -1;

	nodeVal *val9 = new(nodeVal);
	val9->r = makeRect(600,300,700,400);
	val9->child = -1;

	nodeVal *val10 = new(nodeVal);
	val10->r = makeRect(120,320,150,400);
	val10->child = -1;
    
    nodeVal *val11 = new(nodeVal);
	val11->r = makeRect(100,300,150,400);
	val11->child = -1;
    
	RTree *tree = makeTree();
	insert(tree, val);
	insert(tree, val2);
	insert(tree, val3);
    insert(tree, val4);
    insert(tree, val5);
    insert(tree, val6);
    insert(tree, val7);
    fprintf(stderr, "---------------- pase el 7 ----------\n");
    
    insert(tree, val0);
    insert(tree, val8);
    insert(tree, val9);
    insert(tree, val10);
    insert(tree, val11);



    int k;
    rect *r = makeRect(1,1,2000,2000);
    int size_resp = 100;
    twoInts **resp= (twoInts **) malloc(size_resp * sizeof(twoInts *)); 
    k = search(r, tree, resp, size_resp);
    //delete(r, tree, resp[7]);
    //delete(r, tree, resp[8]);
    //delete(r, tree, resp[5]);
    //delete(r, tree, resp[5]);


	freeRTree(tree);
}
