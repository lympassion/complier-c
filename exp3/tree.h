#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define CHILDNUM 20

typedef struct treeNode treeNode; //polytree
struct treeNode{
	char element[35];
    treeNode* child[CHILDNUM];
};

treeNode *construct(treeNode *t, char* ele){ 
    memcpy(t->element, ele, sizeof(t->element));
    for (int i = 0; i < CHILDNUM; i++){
        t->child[i] = NULL;
    }
    return t;   
}

void print(treeNode *root,int blk)
{
	treeNode *t = root;
	
	if (t != NULL)
	{
		for (int i = 0; i<blk; i++){
            printf("|  ");  // indention
		}
		printf("|--%s\n", t->element);
		for (int i = 0; i < CHILDNUM; i++)
		{
			print(t->child[i],blk+1);
		}
	}
}
