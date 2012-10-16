
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include "tarea1.h"
#include "RTree.h"


nodeVal *refreshNode(){
	static int idx = 0;
	nodeVal *aux=new(nodeVal);
	aux->r=randRect();
	aux->child=idx++;
	return aux;
}


int main(int argc, char *argv[]){
	srand48(time(NULL));
	int i;
	nodeVal *data;
	RTree *tree = readTree();

	int m = strtol(argv[1], NULL, 10);

	int insertMethod = strtol(argv[2], NULL, 10);
	setInsertMethod(insertMethod);

	printf("-----------------\n");
	printf("Inserción tipo:  %d\n", insertMethod );
	printf("Número de inserciones:  %d\n", m);

	data=refreshNode();
	for(i=0; i<m; i++){
		insert(tree,data);
		data=refreshNode();
	}
	printf("Lecturas de Archivos: %d\n", getReads());
	printf("Escrituras de Archivos: %d\n", getWrites());

	printf("-----------------\n");

	writeRTree(tree);
}

