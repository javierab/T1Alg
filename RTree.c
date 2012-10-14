#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include "RTree.h"

int insertMethod = 1;

void setInsertMethod(int i){
	insertMethod = i;
}


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

	if(insertMethod == 1)
		selectRect1(n->values, &imax, &jmax);
	else
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
	float delta1, delta2;
	while(count1 < b+1 && count2 < b+1){
        maxdif = 0, idif = 0;
		for (i=0;i<2*b+1;i++){
			if(n->values[i]!=NULL){
				delta1 = deltaMBR(group1->MBR,n->values[i]->r);
				delta2 = deltaMBR(group2->MBR,n->values[i]->r);
				if(delta1 < delta2)
					dif = delta2-delta1;
				else
					dif = delta1-delta2;
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
		int imin = -1;
		float min = HUGE_VAL, amin,mbr;
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
	nodeVal *cpval = new(nodeVal);
       	cpval->r = dupRect(val->r);
       	cpval->child = val->child;

       	val = cpval;


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

int dupResp(int **resp, int size_resp){
    size_resp <<= 1;
    *resp = (int *) realloc(*resp,size_resp*sizeof(int));
    return size_resp;
}

void recSearch(rect *r, node *n, int **resp, int *offset, int size_resp){
    int i;
    node *n1;

    if(n->leaf){
        for(i = 0; i < n->size; ++i){
            if(intersect(r, n->values[i]->r)){
                if(*offset == size_resp)
                    size_resp = dupResp(resp, size_resp);
                (*resp)[(*offset)++] = n->values[i]->child;
            }
        }
        return;
    }

    for(i = 0; i < n->size; ++i){
        if(intersect(r, n->values[i]->r)){
            n1 = readNode(n->values[i]->child);
            recSearch(r, n1, resp, offset, size_resp);
            freeNode(n1);
        }
    }
}

int search(rect *r, RTree *t, int **resp){
    int offset = 0;
    int size_resp = 100;
    *resp= (int *) malloc(size_resp * sizeof(int));
    if(t->root == NULL)
        return 0;
    int i;
    recSearch(r, t->root, resp, &offset, size_resp);
    return offset;
}

void refreshMBR(node *n){
    int i;
    freeRect(n->MBR);
	if(n->size==0){
		n->MBR=makeRect(0,0,0,0);
		return;
	}
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
    refreshMBR(resp);
    destroyNode(n1);
    destroyNode(n2);

	writeNode(resp);
    return resp;
}

void deleteValue(node *n, int i){
    int j;
    freeNodeVal(n->values[i]);
    for(j = i; j < n->size - 1; j++)
        n->values[j] = n->values[j+1];
    n->values[(n->size)-1]=NULL;
    n->size--;
    refreshMBR(n);
}

void underflow(node *n, node *n1, int i){

    int j, sibling;
    node *n2;

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

		refreshMBR(n);
		writeNode(n);

		writeNode(n1);
		freeNode(n1);

		writeNode(n2);
		freeNode(n2);

	}
	else{
		n2 = merge(n1,n2);
		deleteValue(n, sibling);
        freeNodeVal(n->values[i]);
        n->values[i] = new(nodeVal);
		n->values[i]->child = n2->address;
        n->values[i]->r=dupRect(n2->MBR);
        refreshMBR(n);
        writeNode(n);

		freeNode(n2);
	}
}






int recDelete(rect *r, node *n, int pos){
    int i,j;
    node *n1, *n2;
    if(n->leaf){
    	for(i = 0; i < n->size; ++i)
    		if(n->values[i]->child == pos){
            	deleteValue(n, i);

    			return TRUE;
    		}
    	return FALSE;
    }

    for(i = 0; i < n->size; ++i)
        if(intersect(r, n->values[i]->r)){
            n1 = readNode(n->values[i]->child);

            if(recDelete(r, n1, pos)){
                if(n1->size < b )
                	underflow(n, n1, i);
                else
    				freeNode(n1);
                return TRUE;
            }
            else
                freeNode(n1);

        }
    return FALSE;

}

void delete(rect *r, RTree *t, int pos){
	int i;
	recDelete(r, t->root, pos);
	if(t->root->leaf){
		if(t->root->size==0){
			destroyNode(t->root);
			t->root=NULL;
		}
		else
			writeNode(t->root);

	}
	else{
		if(t->root->size == 1){
			node *n = t->root;
			t->root = readNode(t->root->values[0]->child);
			destroyNode(n);
		}
		else
			writeNode(t->root);
	}

}




void insertTree(node *n, RTree *t){
    int i;
    node *n1;

    if(n->leaf){
         for(i = 0; i < n->size; ++i){
            nodeVal *val = new(nodeVal);
            val->r = dupRect(n->values[i]->r);
            val->child = n->values[i]->child;
            insert(t, val);
        }
    }
    else{
        for(i = 0; i < n->size; ++i){
            n1 = readNode(n->values[i]->child);
            insertTree(n1, t);
            destroyNode(n1);
        }
    }
}


int recDelete2(rect *r, node *n, int pos, RTree *t){
    int i,j;
    node *n1, *n2;

    if(n->leaf){
		for(i=0;i<n->size;i++)
			if(n->values[i]->child==pos){
				deleteValue(n, i);
				writeNode(n);
				if(n->size < b){
					return TRUE;

				}
			}
        return FALSE;
    }



    for(i = 0; i < n->size; ++i){
        if(intersect(r, n->values[i]->r)){
            n1 = readNode(n->values[i]->child);
            if(recDelete2(r, n1, pos, t)){

                if(n->size - 1 > b){
                    insertTree(n1, t);
                    destroyNode(n1);
                }
                else{
                    freeNode(n1);
                    return TRUE;
                }
            }
        }
    }
    refreshMBR(n);
    writeNode(n);
    return FALSE;

}

void delete2(rect *r, RTree *t, int pos){
    int i;
    node *n = t->root, *n1;
    if(n->leaf){
		for (i=0; i<n->size; i++) 
			if(n->values[i]->child == pos)
				deleteValue(n, i);
		if(n->size == 0){
			t->root = NULL;
            destroyNode(n);
			return;
		}
		refreshMBR(n);
	writeNode(n);
	return;
    }

    for(i = 0; i < n->size; ++i){
        int s = 0;
        if(intersect(r, n->values[i]->r)){
            n1 = readNode(n->values[i]->child);
            if(recDelete2(r, n1, pos, t)){
                if(n->size - 1 == 1){
                    if(i == 0) s = 1;
                    t->root = readNode(n->values[s]->child);
					destroyNode(n);
                }
                else
                    deleteValue(t->root, i);
                insertTree(n1, t);
                destroyNode(n1);

            }
            else
                freeNode(n1);
        }
		n=t->root;
	}
    refreshMBR(t->root);
    writeNode(t->root);

}
