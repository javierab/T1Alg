#define b 100
#define B 4096
#define TRUE 1
#define FALSE 0

typedef struct{
	float x1;
	float x2;
	float y1;
	float y2;
	int child;
}rect;


typedef struct{
	int size;
	int address;
	rect *values[2*b+1];
	rect *MBR;
	int leaf;
}node;

typedef struct{
	int node1; //nombre de nodo
	int node2;
	rect *rect1;
	rect *rect2;
}insertVal

node *readNode(int i);
void writeNode(node *n);
int intersect(rect *r1,rect *r2);
//rect *makeRect(float x1,float y1, float x2, float y2);
float area(rect *r);
float deltaMBR(rect *MBR, rect *newRect);


