#include "heap_data.h"

struct Node *createNodes(long size)
{
    struct Node *nodes = (struct Node *)malloc(size * sizeof(struct Node));
    for (long i = 0; i < size; i++)
    {
        nodes[i].size_of_nodes = size;
        nodes[i].total = 0;
        nodes[i].size = 0;
        nodes[i].length = 0;
        nodes[i].clazz = NULL;
    }
    // printf("Nodes created successfully. size: %d\n", size);
    return nodes;
}

void addNodeByTag(struct Node *nodes, long class_tag, long size, long length)
{
    if (class_tag < 0 || class_tag >= nodes[0].size_of_nodes)
    {
        fprintf(stderr, "Error: class_tag out of range. max: %ld, current: %ld\n", nodes[0].size_of_nodes, class_tag);
        return;
    }
    nodes[class_tag].total++;
    nodes[class_tag].size += size;
    nodes[class_tag].length += length;
}

