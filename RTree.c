
#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<math.h>
#include "RTree.h"


char *path = "files";

int getNext(){
	static n=0;
	return n++;
}

rect *makeRect(float x1, float y1, float x2, float y2){
	rect *r = (rect *)malloc(sizeof(rect));
	r->x1 = x1;
	r->y1 = y1;
	r->x2 = x2;
	r->y2 = y2;
	return r;
}

node *readNode(int i){
	int child,size,leaf,count;
	float x1,x2,y1,y2;
	char name[20];
	sprintf(name,"%s/%d",path,i);
	FILE *f=fopen(name,"r");
	node *n=(node *)malloc(sizeof(node));
	fscanf(f,"%d,%d", &size, &leaf);
	n->address=i;
	n->leaf =leaf;
	n->size=size;
	for(count=0;count<size;count++){
		fscanf(f,"%f,%f,%f,%f,%d", &x1,&y1,&x2,&y2,&child);
		n->values[count] = makeRect(x1,y1,x2,y2);
		n->values[count]->child = child;
	}
	fclose(f);
	return n;
}



void writeNode(node *n){
	char name[20];
	int j;
	int i=n->address;
	sprintf(name,"%s/%d",path,i);
	FILE *f=fopen(name,"w");
	fprintf(f,"%d,%d\n",n->size, n->leaf);
	for (j=0;j<n->size;j++){
		rect *r=n->values[j];
		fprintf(f,"%f, %f, %f,%f,%d\n",r->x1,r->y1,r->x2,r->y2,r->child);
		
	}
	fclose(f);
}

float area(rect *r){
	return abs(r->x2-r->x1)*abs(r->y2-r->y1);
}

void increaseMBR(rect *MBR, rect *r){
	MBR->x1=fmin(MBR->x1,r->x1);
	MBR->y1=fmin(MBR->y1,r->y1);
	MBR->x2=fmax(MBR->x2,r->x2);
	MBR->y2=fmax(MBR->y2,r->y2);
}

float deltaMBR(rect *MBR, rect *newRect){
//asumiendo que siempre x1<x2 e y1<y2
	rect *newMBR = (rect *)malloc(sizeof(rect));
	newMBR->x1=fmin(MBR->x1,newRect->x1);
	newMBR->y1=fmin(MBR->y1,newRect->y1);
	newMBR->x2=fmax(MBR->x2,newRect->x2);
	newMBR->y2=fmax(MBR->y2,newRect->y2);
	return area(newMBR)-area(MBR);
}


int intersect(rect *r1,rect *r2){
	if( (r1->x1 >= r2->x1 && r1->x1 <= r2->x2 || r1->x2 >= r2->x1 && r1->x2 <= r2->x2 ) && (r1->y1 >= r2->y1 && r1->y1 <= r2->y2 || r1->y2 >= r2->y1 && r1->y2 <= r2->y2 ))
		return TRUE;
	
	if( (r2->x1 >= r1->x1 && r2->x1 <= r1->x2 || r2->x2 >= r1->x1 && r2->x2 <= r1->x2 ) && (r2->y1 >= r1->y1 && r2->y1 <= r1->y2 || r2->y2 >= r1->y1 && r2->y2 <= r1->y2 ))
		return TRUE;
	
	return FALSE;
}

rect *dupRect(rect *r){
	rect *r2=makeRect(r->x1, r->y1, r->x2, r->y2);
	r2->child = r->child;
	return r2;
}

insertVal *split(node *n){
	int max1,max2,i;
	float amax=0;
	for (i=0;i<2*b+1;i++)
		if(area(n->values[i])>amax){
			max1=i;
			amax=area(n->values[i]);
		}
	amax=0;
	for (i=0;i<2*b+1;i++)
		if(area(n->values[i])>amax && i!=max1){
			max2=i;
			amax=area(n->values[i]);
		}			
	node *group1=(node *)malloc(sizeof(node));
	node *group2=(node *)malloc(sizeof(node));
	group1->size=group2->size=0;
	group1->values[group1->size++]=n->values[max1];
	group2->values[group2->size++]=n->values[max2];
	n->values[max1]=NULL;
	n->values[max2]=NULL;
	rect *mbr1=dupRect(group1->values[0]);
	rect *mbr2=dupRect(group2->values[0]);
	float dif=0,maxdif=0;
	int idif;
	int step = 2*b-1;
	while(step-- > 0){
		for (i=0;i<2*b+1;i++)
			if(n->values[i]!=NULL){
				dif=abs(deltaMBR(mbr1,n->values[i])-deltaMBR(mbr2,n->values[i]));
				if(dif>maxdif)
					idif=i;
			}
		if(deltaMBR(mbr1,n->values[idif])<deltaMBR(mbr2,n->values[idif])){
			group1->values[group1->size++]=n->values[idif];
			increaseMBR(mbr1, n->values[idif]);
		}
		else{
			group2->values[group2->size++]=n->values[idif];
			increaseMBR(mbr2, n->values[idif]);
		}	
		n->values[idif] = NULL;
	}
	
	insertVal *ret = (insertVal *)malloc(sizeof(insertVal));
	
	ret->mbr1 = mbr1;
	ret->node1 = getNext();
	ret->mbr2 = mbr2;
	ret->node2 = getNext();
	
	group1->address = ret->node1;
	group2->address = ret->node2;
	
	writeNode(group1);
	writeNode(group2);
	
	//matar cosas 	
	
	return ret;
}


insertVal *insert(node *n, rect *r){
	if(!n->leaf){
		int min = 1 << 20, imin,mbr;
		float amin;
		int i;
		for(i = 0; i < n->size; i++){
			mbr = deltaMBR(n->values[i], r); 
			if(mbr < min){
				min = mbr;
				imin = i;
				amin=area(n->values[i])+mbr;
			}
			else if(mbr == min){
				if((area(n->values[i])+mbr)<amin){
					imin=i;
					amin=area(n->values[i])+mbr;					

				}
					
			}
			
		}
		insertVal *ret = insert(readNode(n->values[imin]->child), r);
		
		//matar cosas
		
		n->values[imin] = ret->mbr1;
		n->values[imin]->child = ret->node1;
		
		if(ret->node2 != -1){
			if(n->size<2*b){
				n->values[n->size] = ret->mbr2;
				n->values[n->size++]->child = ret->node2;
				
			}
			else{
				n->values[2*b] = ret->mbr2;
				n->values[2*b]->child = ret->node2;
				return split(n);
			}
		}
	}
	else {
		if(n->size<2*b){
			n->values[n->size]=r;
			n->size=n->size+1;
			
			insertVal *ret = (insertVal *)malloc(sizeof(insertVal));
			ret->mbr1 = mbr1;
			ret->node1 = n->address;
			return ret;
		}
		else{
			n->values[2*b] = r;
			return split(n);

		}
	}

}


