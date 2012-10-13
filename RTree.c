#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "RTree.h"


RTree *makeTree(){
	return new(RTree);
}

void freeRTree(RTree *t){
	freeNode(t->root);
	free(t);
}

void selectRect1(nodeVal **values, int *imax, int *jmax){
    float max_area;
    rect *mbr;
    int i,j;
	for (i=0;i<2*b+1;i++){
        for (j=i+1;j<2*b+1;j++){
            mbr = dupRect(values[i]->r);
            increaseMBR(mbr, values[j]->r);
            if(area(mbr) > max_area){
                max_area = area(mbr);
                *imax = i, *jmax = j;
            }
            freeRect(mbr);   
        }
    }
}

void selectRect2(nodeVal **values, int *imax, int *jmax){
    float max_dist, d;
    int i,j;
	for (i=0;i<2*b+1;i++){
        for (j=i+1;j<2*b+1;j++){
            if((d = distancia(values[i]->r,values[j]->r)) > max_dist){
                max_dist = d;
                *imax = i, *jmax = j;
            }
                
        }
    }

}

insertVal *split(node *n, int leaf){
	node *group1, *group2;
	insertVal *ret;
    int imax, jmax, i;
	rect *mbr1, *mbr2;
    selectRect2(n->values, &imax, &jmax);


	group1 = makeNode(leaf);
	group1->values[group1->size++] = n->values[imax];
	group1->MBR = dupRect(group1->values[0]->r);
	n->values[imax]=NULL;

	group2 = makeNode(leaf);
	group2->values[group2->size++] = n->values[jmax];
	group2->MBR = dupRect(group2->values[0]->r);
	n->values[jmax]=NULL;


	float dif=0, maxdif=0;
	int idif, steps = 2*b-1;
	while(steps-- > 0){
		for (i=0;i<2*b+1;i++){
			if(n->values[i]!=NULL){
				dif=abs(deltaMBR(group1->MBR,n->values[i]->r)-deltaMBR(group2->MBR,n->values[i]->r));
				if(dif>=maxdif)
                    maxdif = dif, idif=i;
			}
		}
        
		if(deltaMBR(group1->MBR,n->values[idif]->r)<=deltaMBR(group2->MBR,n->values[idif]->r)){
			group1->values[group1->size++] = n->values[idif];
			increaseMBR(group1->MBR, n->values[idif]->r);
		}
		else{
			group2->values[group2->size++] = n->values[idif];
			increaseMBR(group2->MBR, n->values[idif]->r);
		}	
		n->values[idif] = NULL;
	}

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

    if(n->leaf){
		freeNodeVal(n->values[n->size]);
		n->values[n->size++] = val;

		//Si no hay overflow. *n->size aumento*
		if(n->size <= 2*b){
			writeNode(n);
			ret = new(insertVal);
			ret->node1 = n;
			ret->node2 = NULL;	
		}
		else
			ret = split(n, TRUE);

		return ret;
	}
	else{
		int min = 1 << 20, imin,mbr;
		float amin;
		int i;
		for(i = 0; i < n->size; i++){
			r = n->values[i]->r;
			mbr = deltaMBR(r, val->r);

			if(mbr < min){
				min = mbr, imin = i;
				amin = area(r) + mbr;
			}
			else if(mbr == min && (area(r)+mbr)<amin){
				imin=i;
				amin = area(r)+mbr;				
			}
			
		}
        
		increaseMBR(n->MBR, val->r);

		ret = recInsert(readNode(n->values[imin]->child), val);
		n1 = ret->node1;
		n2 = ret->node2;
		free(ret);

		//Colgar primer nodo.
		freeNodeVal(n->values[imin]);
		n->values[imin] = makeNodeVal(dupRect(n1->MBR), n1->address);

		//Si hubo overflow en el hijo colgamos segundo nodo.
		if(ret->node2 != NULL){
			freeNodeVal(n->values[n->size]);
			n->values[n->size++] = makeNodeVal(dupRect(n2->MBR), n2->address);
		}
		//Si no hubo overfow en el hijo ó no hay overflow aquí.
		if(ret->node2 == NULL || n->size <= 2*b){
			writeNode(n);
			ret = new(insertVal);			
			ret->node1 = n;
			ret->node2 = NULL;
		}
		else
			ret = split(n, FALSE);

		freeNode(n1);
		freeNode(n2);
		return ret;
	}
}

void insert(RTree *t, nodeVal *val){
	insertVal *ret;
	node *n1, *n2, *n;

	if(t->root == NULL){
		n = makeNode(TRUE);
		n->values[n->size++] = val;
		n->MBR = dupRect(val->r);
		writeNode(n);

		t->root = n;
	}
	else{
		ret = recInsert(t->root, val);
		n1 = ret->node1;
		n2 = ret->node2;
		free(ret);
		if(n2 != NULL){
			n = makeNode(FALSE);
			n->values[n->size++] = makeNodeVal(dupRect(n1->MBR), n1->address);
			n->values[n->size++] = makeNodeVal(dupRect(n2->MBR), n2->address);
			n->MBR = dupRect(n1->MBR);
			increaseMBR(n->MBR, n2->MBR);

			writeNode(n);
			t->root = n;

			freeNode(n1);
			freeNode(n2);
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



