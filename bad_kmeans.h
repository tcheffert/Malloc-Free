#ifndef BAD_KMEANS_H
#define BAD_KMEANS_H

#include <stdlib.h>
#include <stdint.h>


extern void*(*malloc_impl)(size_t); // Malloc implementation
extern void(*free_impl)(void*); // Free implementation


typedef struct {
    int64_t* coords;
    int dim;
} point_t;


typedef struct {
    point_t** points;
    uint64_t length;
} points_array_t;


typedef struct linked_cluster {
    point_t* point;
    struct linked_cluster* next;
    uint64_t length;
} linked_cluster_t;


typedef struct {
    linked_cluster_t** clusters;
    int length;
} clusters_array_t;


points_array_t* kmeans(points_array_t* points, uint64_t n, int k, int d);

void freePoints(points_array_t* points);


#endif