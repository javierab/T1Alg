#include "tarea1.h"

typedef struct{
	float x1;
	float x2;
	float y1;
	float y2;
}rect;


rect *makeRect(float x1, float y1, float x2, float y2);
float distancia(rect *r1, rect *r2);
void freeRect(rect *r);
rect *dupRect(rect *r);
float area(rect *r);
int intersect(rect *r1,rect *r2);
float deltaMBR(rect *MBR, rect *newRect);
rect *increaseMBR(rect *MBR, rect *newRect);
void printRect(rect *r);
