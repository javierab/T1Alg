
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include "tarea1.h"
#include "RTree.h"
//#include<gsl/gsl_randist.h>
//#include <gsl/gsl_rng.h>

#define nTotal 512 /*2^9*/
#define size_resp 100
int idx;

rect *randRect(){
	float x1=drand48()*500000;
	float y1=drand48()*500000;
	float size=drand48()*99 +1;
	float x2= x1+size;
	size=drand48()*99 +1;
	float y2= y1+size;
	while (y2>500000){
		y1=drand48()*500000;
		y2= y1+size;
	}
	while (x2>500000){
		x1=drand48()*500000;
		size=drand48()*99 +1;
		x2= x1+size;
	}
	return makeRect(x1,y1,x2,y2);

}

void randDelete(node *no, RTree *t){
	int j = round(drand48()*(no->size-1));
	if(no==NULL)
		fprintf(stderr,"Intento de acceso a nodo nulo");
	
	if(no->leaf){
		rect *r1=dupRect(no->values[j]->r);
		delete(r1,t,no->values[j]->child);	
	}
	else{
		node *n2=readNode(no->values[j]->child);
		randDelete(n2,t);
		freeNode(n2);
	}

}

void randSearch(RTree *t, int ** answer){
	rect *r=randRect();
	search(r,t,answer);

}

nodeVal *refreshNode(){
	nodeVal *aux=new(nodeVal);
	aux->r=randRect();
	aux->child=idx++;
	return aux;
}


int main(){
	srand48(3);//iniciado en 3 da sg fa
	int i;
	RTree *tree = makeTree();	
	nodeVal *data;
	int *resp;
	idx=0;
	data=refreshNode();
	int k=1;
	int j,m;
	for (k=1; k<8; k=2*k+1) {
		m=nTotal/(2*k);
		for (j=0; j<m; j++) {
			for(i=0;i<k;i++){
				insert(tree,data);
				data=refreshNode();
			}
			for(i=0;i<k;i++)
				randDelete(tree->root,tree);
			for(i=0;i<k;i++){
				insert(tree,data);
				data=refreshNode();
			}
		}
		for(j=0;j<m;j++)
			for(i=0;i<k;i++)
				randSearch(tree,&resp);
	
		for (j=0; j<m; j++) {
			for(i=0;i<k;i++)
				randDelete(tree->root,tree);
			for(i=0;i<k;i++){
				insert(tree,data);
				data=refreshNode();
			}
			for(i=0;i<k;i++)
				randDelete(tree->root,tree);
		}
	}
	
}

