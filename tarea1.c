

#include "tarea1.h"
#include <stdio.h>
#include "RTree.h"

int main(){
	//mantener un int con la cantidad de nodos
	
	node *n =readNode(1);

	n->address=3;
	writeNode(n);
}
