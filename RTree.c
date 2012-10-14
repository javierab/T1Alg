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
    int imax, jmax, i,j;
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
	int idif, count1 = 1, count2 = 1;
	while(count1 < b+1 && count2 < b+1){
        maxdif = 0, idif = 0;
		for (i=0;i<2*b+1;i++){
			if(n->values[i]!=NULL){
				dif=abs(deltaMBR(group1->MBR,n->values[i]->r)-deltaMBR(group2->MBR,n->values[i]->r));
				if(dif>=maxdif)
                    maxdif = dif, idif=i;
			}
		}
		if(deltaMBR(group1->MBR,n->values[idif]->r)<=deltaMBR(group2->MBR,n->values[idif]->r)){
			group1->values[group1->size++] = n->values[idif];
            count1++;
			increaseMBR(group1->MBR, n->values[idif]->r);
		}
		else{
			group2->values[group2->size++] = n->values[idif];
            count2++;
			increaseMBR(group2->MBR, n->values[idif]->r);
		}
		n->values[idif] = NULL;


	}
    
    while(count1 < b){
        for (i=0;i<2*b+1;i++){
			if(n->values[i]!=NULL){
				group1->values[count1++] = n->values[i];
                increaseMBR(group1->MBR, n->values[i]->r);
                n->values[i] = NULL;
                group1->size++;
			}
		}
    }
    while(count2 < b){
        for (i=0;i<2*b+1;i++){
			if(n->values[i]!=NULL){
				group2->values[count2++] = n->values[i];
                increaseMBR(group2->MBR, n->values[i]->r);
                n->values[i] = NULL;
                group2->size++;
			}
		}
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
            increaseMBR(n->MBR, val->r);
			writeNode(n);
			ret = new(insertVal);
			ret->node1 = n;
			ret->node2 = NULL;	
		}
		else{

			ret = split(n, TRUE);
        }
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
		if(n2 != NULL){
			freeNodeVal(n->values[n->size]);
			n->values[n->size++] = makeNodeVal(dupRect(n2->MBR), n2->address);

		}

		//Si no hubo overflow en el hijo o no hay overflow aquí.
		if(n2 == NULL || n->size <= 2*b){
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



int dupResp(twoInts **resp, int size_resp){
    size_resp <<= 1;
    resp = (twoInts **) realloc(resp,size_resp*sizeof(twoInts *));
    return size_resp;
}

void recSearch(rect *r, node *n, twoInts **resp, int *offset, int size_resp){
    int i;
    node *n1;
    if(n->leaf){
        for(i = 0; i < n->size; ++i){
            if(intersect(r, n->values[i]->r)){
                twoInts *twoints = new(twoInts);
                twoints->int1 = n->address;
                twoints->int2 = i;
                if(*offset == size_resp)
                    size_resp = dupResp(resp, size_resp);
                resp[(*offset)++] = twoints; 
                fprintf(stderr,"Intersecta con: %d,%d\n\n", resp[(*offset)-1]->int1, resp[(*offset)-1]->int2);
            }
        }
        return;
    }

    for(i = 0; i < n->size; ++i)
        if(intersect(r, n->values[i]->r)){
            n1 = readNode(n->values[i]->child);
            recSearch(r, n1, resp, offset, size_resp);
            freeNode(n1);
        }
}

int search(rect *r, RTree *t, twoInts **resp, int size_resp){
    int offset = 0;
    if(t->root == NULL)
        return 0;
    int i;
    recSearch(r, t->root, resp, &offset, size_resp);

    return offset;
}

void refreshMBR(node *n){
    int i;
    freeRect(n->MBR);
    n->MBR = dupRect(n->values[0]->r);
    for(i=1; i < n->size; ++i)
           increaseMBR(n->MBR, n->values[i]->r);
}

node *merge(node *n1, node *n2){
    int i;
    node *resp = makeNode(n1->leaf);
    for(i = 0; i < n1->size; i++){
        resp->values[i] = n1->values[i];
        n1->values[i] = NULL;
    }
    for(i = 0; i < n2->size; i++){
        resp->values[n1->size + i] = n2->values[i];
        n2->values[i] = NULL;
    }
        
    resp->size = n1->size + n2->size;
    
    for(i = 0; i < resp->size; i++){
        fprintf(stderr, "values(%d) = %x\n\n", i, resp->values[i]);
    }
    refreshMBR(resp);
    destroyNode(n1);
    destroyNode(n2);

    return resp;
}

void underflow(node *n, node *n1, int i){

    int j, sibling;
    node *n2;
    
    if(n1->size < b ){
        if(n->size == i + 1){
            n2 = readNode(n->values[i-1]->child);
            sibling = i-1;
        }
        else{
            n2 = readNode(n->values[i+1]->child);
            sibling = i+1;
        }
        if(n2->size > b){
            n1->values[n1->size++] = n2->values[--(n2->size)];
            refreshMBR(n1);
            refreshMBR(n2);
            freeRect(n->values[i]->r);
            n->values[i]->r = dupRect(n1->MBR);
            freeRect(n->values[sibling]->r);
            n->values[sibling]->r = dupRect(n2->MBR);
                            
            writeNode(n);writeNode(n1);writeNode(n2);
                            
        }
        else{
            n2 = merge(n1,n2);
            n->values[i]->child = n2->address;
            freeNodeVal(n->values[sibling]);
            for(j = sibling; j < n->size - 1; j++)
                n->values[j] = n->values[j+1];
            n->size--;
            n->values[n->size]= NULL;            
            writeNode(n2);
        }
    }
}






int recDelete(rect *r, node *n, twoInts *pos){
    int i,j;
    node *n1, *n2;

    for(i = 0; i < n->size; ++i)
        if(intersect(r, n->values[i]->r)){
            n1 = readNode(n->values[i]->child);
            if(n1->leaf){
                if(n1->address == pos->int1){
                    freeNodeVal(n1->values[pos->int2]);
                    for(j = pos->int2; j < n1->size - 1; j++)
                        n1->values[j] = n1->values[j+1];
                    n1->values[(n->size)-1]=NULL;
                    n1->size--;
                    underflow(n, n1, i);
                    refreshMBR(n);
                    writeNode(n);
                    return TRUE;
                }
            }
            else{
                if(recDelete(r, n1, pos)){
                    underflow(n, n1, i);
                    refreshMBR(n);
                    writeNode(n);
                    return TRUE;
                }
            }
            freeNode(n1);
        }
    return FALSE;
    
}

void delete(rect *r, RTree *t, twoInts *pos){
    recDelete(r, t->root, pos);
    if(t->root->size == 1){
        node *n = t->root;
        t->root = readNode(t->root->values[0]->child);
        destroyNode(n);
    }
}



