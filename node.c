#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "node.h"



char *path = "files";


int getNextAdress(){
	static int n=0;
	return n++;
}


node *makeNode(int leaf){
	node *n=new(node);
	n->address = getNextAdress();
	n->size=0;
    n->leaf = leaf;
	n->MBR = NULL;
    return n;
}
node *readNode(int address){
	FILE *f;
	node *n;
	int child,size,leaf,i;
	float x1,x2,y1,y2;
	char filename[20];

	sprintf(filename,"%s/%d",path,address);
	errno = 0;
	f=fopen(filename,"r");

	if(f == NULL){
		fprintf(stderr, "Error al abrir el archivo.\n");
		exit(1);
	}

	n=new(node);
	n->address=address;

	//size,leaf
	fscanf(f,"%d,%d", &size, &leaf);
	n->leaf = leaf;
	n->size = size;

	//mbr
	fscanf(f,"%f,%f,%f,%f", &x1,&y1,&x2,&y2);
	n->MBR = makeRect(x1,y1,x2,y2);
    int a;
	for(i=0;i<size;i++){
		a = fscanf(f,"%f,%f,%f,%f,%d", &x1,&y1,&x2,&y2,&child);
        n->values[i] = new(nodeVal);
		n->values[i]->r = makeRect(x1,y1,x2,y2);
		n->values[i]->child = child;
	}
	fclose(f);
    
	return n;
}



void writeNode(node *n){
	char filename[20];
	int i;
	sprintf(filename,"%s/%d",path,n->address);
	FILE *f=fopen(filename,"w");

	if(f == NULL){
		fprintf(stderr, "Error al abrir el archivo.\n");
		exit(1);
	}
	//Size,leaf
	fprintf(f,"%d,%d\n",n->size, n->leaf);

	//MBR
	fprintf(f,"%f,%f,%f,%f\n",n->MBR->x1, n->MBR->y1, n->MBR->x2, n->MBR->y2);

	for (i=0;i<n->size;i++){
		rect *r=n->values[i]->r;
		fprintf(f,"%f,%f,%f,%f,%d\n",r->x1, r->y1, r->x2, r->y2, n->values[i]->child);
	}
	fclose(f);
}


void freeNode(node *n){
	int i;
	if(n != NULL){
		for(i=0; i<n->size; ++i)
			freeNodeVal(n->values[i]);
		freeRect(n->MBR);
		free(n);
	}
}

void destroyNode(node *n){
	char filename[20];
	sprintf(filename,"%s/%d",path,n->address);
	unlink(filename);
	freeNode(n);
}

void freeNodeVal(nodeVal *v){
	if(v != NULL){
		freeRect(v->r);
		free(v);
	}
}

nodeVal *makeNodeVal(rect *r, int child){
	nodeVal *val = new(nodeVal);
	val->r = r;
	val->child = child;
	return val;
}



