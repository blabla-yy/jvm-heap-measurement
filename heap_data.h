#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef _STD_LIB_H
#define _STD_LIB_H
#include <stdlib.h>
#endif

#ifndef JNI_H
#define JNI_H
#include <jni.h>
#endif

struct Node
{
    long total;
    long size;
    long length;
    long size_of_nodes;
    jclass clazz;
};

struct Node *createNodes(long size);
void addNodeByTag(struct Node *nodes, long class_tag, long size, long length);