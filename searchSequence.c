
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include "tarea1.h"
#include "RTree.h"


void randSearch(RTree *t, int ** answer){
	rect *r=randRect();
	search(r,t,answer);

}

int main(int argc, char *argv[]){
	srand48(time(NULL));
	int i, *resp;
	RTree *tree = readTree();

	int m = strtol(argv[1], NULL, 10);

	printf("-----------------\n");
	printf("Número de búsquedas:  %d\n", m);
	for(i=0;i<m;i++)
		randSearch(tree,&resp);

	printf("Lecturas de Archivos: %d\n", getReads());
	printf("Escrituras de Archivos: %d\n", getWrites());

	printf("-----------------\n");


	writeRTree(tree);
}

