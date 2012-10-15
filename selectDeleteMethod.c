
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include "tarea1.h"
#include "RTree.h"
//#include<gsl/gsl_randist.h>
//#include <gsl/gsl_rng.h>

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

void randDelete(node *no, RTree *t, int deletion){
	int j = round(drand48()*(no->size-1));
	if(no==NULL)
		fprintf(stderr,"Intento de acceso a nodo nulo");

	if(no->leaf){
		rect *r1=dupRect(no->values[j]->r);
		if(deletion == 1)
			delete(r1,t,no->values[j]->child);
		else
			delete2(r1,t,no->values[j]->child);
	}
	else{
		node *n2=readNode(no->values[j]->child);
		randDelete(n2,t, deletion);
		freeNode(n2);
	}

}

void randSearch(RTree *t, int ** answer){
	static int n = 0;
	rect *r=randRect();
	search(r,t,answer);

}

nodeVal *refreshNode(){
	nodeVal *aux=new(nodeVal);
	aux->r=randRect();
	aux->child=idx++;
	return aux;
}


int main(int argc, char *argv[]){
	srand48(time(NULL));
	int i;
	RTree *tree = makeTree();
	nodeVal *data;
	int *resp;
	idx=0;
	data=refreshNode();
	int k=1;
	int j,m;

	int shift = strtol(argv[1],NULL, 10);

	int nTotal = 1 << shift;

	int deletion = strtol(argv[2], NULL, 10);

	time_t ini;
	time_t fin;

	printf("********\n");
	printf("n = %d\n",nTotal);
	printf("********\n\n");



	setInsertMethod(1);
	printf("Deletion method: %d\n", deletion);
	printf("-----------------\n\n");
	for (k=1; k<8; k=2*k+1) {
		m=nTotal/(2*k);
		printf("k = %d, m = %d\n",k, m);
		time(&ini);
		for (j=0; j<m; j++) {
			for(i=0;i<k;i++){
				insert(tree,data);
				data=refreshNode();
			}
			for(i=0;i<k;i++)
				randDelete(tree->root,tree, deletion);
			for(i=0;i<k;i++){
					insert(tree,data);
					data=refreshNode();
				}
		}
		time(&fin);
		printf("Tiempo inserciones y borrados: %d\n", fin-ini);

		time(&ini);
		for(j=0;j<m;j++)
			for(i=0;i<k;i++)
				randSearch(tree,&resp);

		time(&fin);
		printf("Tiempo Busquedas: %d\n", fin-ini);
		printf("-----------------\n");

		tree->root = NULL;
	}

	return 0;
}

