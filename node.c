#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>


#include "node.h"



char *path = "files";


int getNextAdress(){
	static int n=0;
	return n++;
}


node *makeNode(int leaf){
    int i;
	node *n=new(node);
	n->address = getNextAdress();
	n->size=0;
    n->leaf = leaf;
	n->MBR = NULL;
    for(i = 0; i < 2*b+1; i++)
        n->values[i]=NULL;
    return n;
}
//node *readNode(int address){
//	FILE *f;
//	node *n;
//	int child,size,leaf,i;
//	float x1,x2,y1,y2;
//	char filename[20];
//
//	sprintf(filename,"%s/%d",path,address);
//	errno = 0;
//	f=fopen(filename,"r");
//
//	if(f == NULL){
//		fprintf(stderr, "Error al leer el archivo: %s\n", filename);
//		exit(1);
//	}
//
//	n=new(node);
//	n->address=address;
//
//    for(i = 0; i < 2*b+1; i++)
//        n->values[i]=NULL;
//
//	//size,leaf
//	fscanf(f,"%d,%d", &size, &leaf);
//	n->leaf = leaf;
//	n->size = size;
//
//	//mbr
//	fscanf(f,"%f,%f,%f,%f", &x1,&y1,&x2,&y2);
//	n->MBR = makeRect(x1,y1,x2,y2);
//    int a;
//	for(i=0;i<size;i++){
//		a = fscanf(f,"%f,%f,%f,%f,%d", &x1,&y1,&x2,&y2,&child);
//        n->values[i] = new(nodeVal);
//		n->values[i]->r = makeRect(x1,y1,x2,y2);
//		n->values[i]->child = child;
//	}
//	fclose(f);
//
//	return n;
//}

node *readNode(int address){
	FILE *f;
	node *n;
	int child,size,leaf,i;
	float x1,x2,y1,y2;
	char filename[20];

	sprintf(filename,"%s/%d",path,address);
	errno = 0;
	f=fopen(filename,"rb");
	if(f == NULL){
		fprintf(stderr, "Error al leer el archivo %s: %s\n", filename, strerror(errno));
		exit(1);
	}

	n=new(node);
	n->address=address;

    for(i = 0; i < 2*b+1; i++)
        n->values[i]=NULL;

	//size,leaf
	fread(&size,4, 1, f);
	fread(&leaf,4, 1, f);
	n->leaf = leaf;
	n->size = size;

	//mbr
	fread(&x1, 4, 1, f);
	fread(&y1, 4, 1, f);
	fread(&x2, 4, 1 ,f);
	fread(&y2, 4, 1, f);

	n->MBR = makeRect(x1,y1,x2,y2);
    int a;
	for(i=0;i<size;i++){
		fread(&x1, 4, 1, f);
		fread(&y1, 4, 1, f);
		fread(&x2, 4, 1 ,f);
		fread(&y2, 4, 1, f);
		fread(&child, 4, 1, f);
        n->values[i] = new(nodeVal);
		n->values[i]->r = makeRect(x1,y1,x2,y2);
		n->values[i]->child = child;
	}
	fclose(f);
	return n;
}

//void writeNode(node *n){
//	char filename[20];
//	int i;
//	sprintf(filename,"%s/%d",path,n->address);
//	FILE *f=fopen(filename,"w");
//
//	if(f == NULL){
//		fprintf(stderr, "Error al escribir archivo: %s\n", filename);
//		exit(1);
//	}
//	//Size,leaf
//	fprintf(f,"%d,%d\n",n->size, n->leaf);
//
//	//MBR
//	fprintf(f,"%f,%f,%f,%f\n",n->MBR->x1, n->MBR->y1, n->MBR->x2, n->MBR->y2);
//
//	for (i=0;i<n->size;i++){
//		rect *r=n->values[i]->r;
//		fprintf(f,"%f,%f,%f,%f,%d\n",r->x1, r->y1, r->x2, r->y2, n->values[i]->child);
//	}
//	fclose(f);
//}

void writeNode(node *n){
	char filename[20];
	int i;
	sprintf(filename,"%s/%d",path,n->address);

	errno = 0;
	FILE *f=fopen(filename,"wb");

	if(f == NULL){
		fprintf(stderr, "Error al leer escribir archivo %s: %s\n", filename, strerror(errno));
		exit(1);
	}
	//Size,leaf
	fwrite(&(n->size), 4, 1 ,f);
	fwrite(&(n->leaf), 4, 1 ,f);

	//MBR
	fwrite(&(n->MBR->x1), 4, 1 ,f);
	fwrite(&(n->MBR->y1), 4, 1 ,f);
	fwrite(&(n->MBR->x2), 4, 1 ,f);
	fwrite(&(n->MBR->y2), 4, 1 ,f);

	for (i=0;i<n->size;i++){
		rect *r=n->values[i]->r;
		fwrite(&(r->x1), 4, 1 ,f);
		fwrite(&(r->y1), 4, 1 ,f);
		fwrite(&(r->x2), 4, 1 ,f);
		fwrite(&(r->y2), 4, 1 ,f);
		fwrite(&(n->values[i]->child), 4, 1, f);
	}
	fclose(f);
}



void freeNode(node *n){
	int i;
	if(n != NULL){
		for(i=0; i<n->size; ++i){
			freeNodeVal(n->values[i]);

        }
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



