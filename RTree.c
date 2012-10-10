#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "RTree.h"


char *path = "files";

RTree *makeTree(){
	return new(RTree);
}


int getNext(){
	static n=0;
	return n++;
}



node *readNode(int address){
	FILE *f;
	node *n;
	int child,size,leaf,i;
	float x1,x2,y1,y2;
	char filename[20];

	sprintf(filename,"%s/%d",path,address);
	errno = 0;
	f=fopen(filename,"r");

	if(f == NULL){
		fprintf(stderr, "Error al abrir el archivo.\n");
		exit(1);
	}

	n=new(node);
	n->address=address;

	//size,leaf
	fscanf(f,"%d,%d", &size, &leaf);
	n->leaf = leaf;
	n->size = size;

	//mbr
	fscanf(f,"%f,%f,%f,%f", &x1,&y1,&x2,&y2);
	n->MBR = makeRect(x1,y1,x2,y2);
    int a;
	for(i=0;i<size;i++){
		a = fscanf(f,"%f,%f,%f,%f,%d", &x1,&y1,&x2,&y2,&child);
        n->values[i] = new(nodeVal);
		n->values[i]->r = makeRect(x1,y1,x2,y2);
		n->values[i]->child = child;
	}
    
	fclose(f);
    
	return n;
}



void writeNode(node *n){
	char filename[20];
	int i;
	sprintf(filename,"%s/%d",path,n->address);
	FILE *f=fopen(filename,"w");

	if(f == NULL){
		fprintf(stderr, "Error al abrir el archivo.\n");
		exit(1);
	}
	//Size,leaf
	fprintf(f,"%d,%d\n",n->size, n->leaf);

	//MBR
	fprintf(f,"%f, %f, %f,%f\n",n->MBR->x1, n->MBR->y1, n->MBR->x2, n->MBR->y2);

	for (i=0;i<n->size;i++){
		rect *r=n->values[i]->r;
		fprintf(f,"%f, %f, %f,%f,%d\n",r->x1, r->y1, r->x2, r->y2, n->values[i]->child);
	}
	fclose(f);
}

void freeNodeVal(nodeVal *v){
	if(v != NULL){
		freeRect(v->r);
		free(v);
	}
}

//Libera la memoria
void freeNode(node *n){
	int i;
	if(n != NULL){
		for(i=0; i<n->size; ++i)
			freeNodeVal(n->values[i]);
		freeRect(n->MBR);
		free(n);
	}
}

//Libera la memoria principal y secundaria
void destroyNode(node *n){
	char filename[20];
	freeNode(n);
	sprintf(filename,"%s/%d",path,n->address);
	unlink(filename);
}

twoInts *selectRect1(nodeVal **values){
    float max_area;
    twoInts *resp = new(twoInts);
    rect *mbr = NULL;
    int i,j;
	for (i=0;i<2*b+1;i++){
        for (j=i+1;j<2*b+1;j++){
            freeRect(mbr);
            mbr = dupRect(values[i]->r);
            increaseMBR(mbr, values[j]->r);
            if(area(mbr) > max_area){
                max_area = area(mbr);
                resp->int1 = i;
                resp->int2 = j;
            }
                
        }
    }
    return resp;
}

twoInts *selectRect2(nodeVal **values){
    float max_dist, d;
    twoInts *resp = new(twoInts);
    int i,j;
	for (i=0;i<2*b+1;i++){
        for (j=i+1;j<2*b+1;j++){
                if((d = distancia(values[i]->r,values[j]->r)) > max_dist){
                max_dist = d;
                resp->int1 = i;
                resp->int2 = j;
            }
                
        }
    }
    return resp;

}


insertVal *split(node *n, int leaf){
	node *group1, *group2;
	insertVal *ret;
    int imax, jmax, i;
    twoInts *resp = selectRect2(n->values);
    imax = resp->int1;
    jmax = resp->int2;
    free(resp);
        
    rect *r1 = n->values[imax]->r;

    rect *r2 = n->values[jmax]->r;
    
	group1=new(node);
	group2=new(node);


	group1->address = getNext();
	group2->address = getNext();

	group1->size=group2->size=0;
	group1->values[group1->size++] = n->values[imax];
	group2->values[group2->size++] = n->values[jmax];


	n->values[imax]=NULL;
	n->values[jmax]=NULL;
	rect *mbr1=dupRect(group1->values[0]->r);
	rect *mbr2=dupRect(group2->values[0]->r);
	float dif=0,maxdif;
    maxdif=0;
	int idif;
	int step = 2*b-1;
	while(step-- > 0){

		for (i=0;i<2*b+1;i++){
			if(n->values[i]!=NULL){
				dif=abs(deltaMBR(mbr1,n->values[i]->r)-deltaMBR(mbr2,n->values[i]->r));
                
				if(dif>=maxdif){
                    maxdif = dif;
					idif=i;
                }
			}
		}
        
		if(deltaMBR(mbr1,n->values[idif]->r)<=deltaMBR(mbr2,n->values[idif]->r)){
			group1->values[group1->size++] = n->values[idif];
			increaseMBR(mbr1, n->values[idif]->r);
		}
		else{
			group2->values[group2->size++] = n->values[idif];
			increaseMBR(mbr2, n->values[idif]->r);
		}	
		n->values[idif] = NULL;
	}
	group1->MBR = mbr1;
	group2->MBR = mbr2;
    group1->leaf = leaf;
    group2->leaf = leaf;

	ret = new(insertVal);
	
	ret->node1 = group1;
	ret->node2 = group2;
	
	destroyNode(n);

	writeNode(group1);
	writeNode(group2);
	
	return ret;
}

insertVal *recInsert(node *n, nodeVal *val){
	rect *r;
	node *n1, *n2;
	insertVal *ret;
    
    

	if(!n->leaf){
		int min = 1 << 20, imin,mbr;
		float amin;
		int i;
		for(i = 0; i < n->size; i++){
			r = n->values[i]->r;
			mbr = deltaMBR(r, val->r);

			if(mbr < min){
				min = mbr;
				imin = i;
				amin=area(r)+mbr;
			}
			else if(mbr == min){
				if((area(r)+mbr)<amin){
					imin=i;
					amin=area(r)+mbr;

				}
					
			}
			
		}
        
		increaseMBR(n->MBR, val->r);

		ret = recInsert(readNode(n->values[imin]->child), val);
		n1 = ret->node1;
		n2 = ret->node2;
		free(ret);
		
		if(ret->node2 == NULL){
        
			freeNodeVal(n->values[imin]);
			n->values[imin] = new(nodeVal);
			n->values[imin]->r = dupRect(n1->MBR);
			n->values[imin]->child = n1->address;

			writeNode(n);

			ret = new(insertVal);
			ret->node1 = n;
			ret->node2 = NULL;

		}
		else{
			if(n->size<2*b){
				freeNodeVal(n->values[n->size]);
				n->values[n->size] = new(nodeVal);
				n->values[n->size]->r = dupRect(n2->MBR);
				n->values[n->size++]->child = n2->address;

				writeNode(n);

				ret = new(insertVal);
				ret->node1 = n;
				ret->node2 = NULL;
			}
			else{
                freeNodeVal(n->values[imin]);
                n->values[imin] = new(nodeVal);
                n->values[imin]->r = dupRect(n1->MBR);
                n->values[imin]->child = n1->address;
            
            
				freeNodeVal(n->values[2*b]);
				n->values[2*b] = new(nodeVal);
				n->values[2*b]->r = dupRect(n2->MBR);
				n->values[2*b]->child = n2->address;

				ret = split(n, FALSE);
			}
		}

		freeNode(n1);
		freeNode(n2);
		return ret;
	}
	else {
		if(n->size<2*b){
			freeNodeVal(n->values[n->size]);
			n->values[n->size++] = val;

			ret = new(insertVal);
			ret->node1 = n;
			ret->node2 = NULL;

			writeNode(n);
			return ret;
		}
		else{
			freeNodeVal(n->values[2*b]);
			n->values[2*b] = val;
			return split(n, TRUE);

		}
	}

}


void insert(RTree *t, nodeVal *val){
	insertVal *ret;

	if(t->root == NULL){
		node *n = new(node);
		n->address = getNext();
		n->MBR = dupRect(val->r);
		n->leaf = TRUE;
		n->size = 1;
		n->values[0] = val;
		writeNode(n);
		t->root = n;
	}
	else{
		ret = recInsert(t->root, val);
		if(ret->node2 != NULL){
			node *n = new(node);
			n->address = getNext();
			n->MBR = dupRect(ret->node1->MBR);
			increaseMBR(n->MBR, ret->node2->MBR);
			n->leaf = FALSE;

			n->values[0] = new(nodeVal);
			n->values[0]->child = ret->node1->address;
			n->values[0]->r = dupRect(ret->node1->MBR);

			n->values[1] = new(nodeVal);
			n->values[1]->child = ret->node2->address;
			n->values[1]->r = dupRect(ret->node2->MBR);

			n->size = 2;

			writeNode(n);
			t->root = n;
		}
	}
}


int size_resp = 100;

void dupResp(twoInts **resp){
    size_resp <<= 1;
    resp = (twoInts **) realloc(resp,size_resp*sizeof(twoInts *));
}

void recSearch(rect *r, node *n, twoInts **resp, int *offset){
    int i;
    node *n1;
    if(n->leaf){
        for(i = 0; i < n->size; ++i){
            if(intersect(r, n->values[i]->r)){
                twoInts *twoints = new(twoInts);
                twoints->int1 = n->address;
                twoints->int2 = i;
                if(*offset == size_resp)
                    dupResp(resp);
                resp[(*offset)++] = twoints; 
                fprintf(stderr,"Intersecta con: %d,%d\n\n", twoints->int1, twoints->int2);
            }
        }
        return;
    }

    for(i = 0; i < n->size; ++i)
        if(intersect(r, n->values[i]->r)){
            n1 = readNode(n->values[i]->child);
            recSearch(r, n1, resp, offset);
            freeNode(n1);
        }
}

int search(rect *r, RTree *t, twoInts **resp){
    resp = (twoInts **) malloc(size_resp*sizeof(twoInts *)); 
    int offset = 0;
    if(t->root == NULL)
        return 0;
    int i;
    recSearch(r, t->root, resp, &offset);
    return offset;
}



