#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "rect.h"

rect *makeRect(float x1, float y1, float x2, float y2){
	rect *r = new(rect);
	r->x1 = x1;
	r->y1 = y1;
	r->x2 = x2;
	r->y2 = y2;
	return r;
}

float distancia(rect *r1, rect *r2){
    float xc1 = (r1->x1+r1->x2)/2;
    float yc1 = (r1->y1+r1->y2)/2;
    float xc2 = (r2->x1+r2->x2)/2;
    float yc2 = (r2->y1+r2->y2)/2;

    return sqrt((xc1-xc2)*(xc1-xc2)+(yc1-yc2)*(yc1-yc2));
}




void freeRect(rect *r){
	if(r != NULL)
		free(r);
}

float area(rect *r){
	return (r->x2 - r->x1)*(r->y2 - r->y1);
}

void printRect(rect *r){
    fprintf(stderr, "(%f,%f) (%f,%f)\n",r->x1,r->y1, r->x2, r->y2);
}


rect *increaseMBR(rect *MBR, rect *r){
	//asumiendo que siempre x1<x2 e y1<y2
	MBR->x1=fminf(MBR->x1,r->x1);
	MBR->y1=fminf(MBR->y1,r->y1);
	MBR->x2=fmaxf(MBR->x2,r->x2);
	MBR->y2=fmaxf(MBR->y2,r->y2);

	return MBR;
}

float deltaMBR(rect *MBR, rect *newRect){
	//asumiendo que siempre x1<x2 e y2<y1
	rect newMBR;
	newMBR.x1=fminf(MBR->x1,newRect->x1);
	newMBR.y1=fminf(MBR->y1,newRect->y1);
	newMBR.x2=fmaxf(MBR->x2,newRect->x2);
	newMBR.y2=fmaxf(MBR->y2,newRect->y2);
	return area(&newMBR)-area(MBR);
}

rect *dupRect(rect *r){
	rect *r2 = makeRect(r->x1, r->y1, r->x2, r->y2);
	return r2;
}


int inside(rect *r, float x, float y){
	return x >= r->x1 && x <= r->x2 && y >= r->y1 && y <= r->y2;
}


int intersect(rect *r1,rect *r2){
	if(inside(r1, r2->x1, r2->y1) || inside(r1, r2->x1, r2->y2) || inside(r1, r2->x2, r2->y1) || inside(r1, r2->x2, r2->y2))
		return TRUE;

	if(inside(r2, r1->x1, r1->y1) || inside(r2, r1->x1, r1->y2) || inside(r2, r1->x2, r1->y1) || inside(r2, r1->x2, r1->y2))
		return TRUE;


	return FALSE;
}



