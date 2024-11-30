#include <stdint.h>
#include <string.h>
#include <limits.h>

#include "bad_kmeans.h"


clusters_array_t* computeClusters(points_array_t* points, uint64_t n, points_array_t* centroids, int k, int d) {
    clusters_array_t* clusters = malloc_impl(sizeof(clusters_array_t));
    clusters->length = k;
    clusters->clusters = malloc_impl(sizeof(linked_cluster_t) * k);
    for (int i = 0; i < k; i++) {
        clusters->clusters[i] = malloc_impl(sizeof(linked_cluster_t));
        clusters->clusters[i]->point = NULL;
        clusters->clusters[i]->next = NULL;
        clusters->clusters[i]->length = 0;
    }
    for (int i = 0; i < n; i++) {
        point_t* point = points->points[i];
        int min = 0;
        int minDist = INT_MAX;
        for (int j = 0; j < k; j++) {
            point_t* centroid = centroids->points[j];
            int dist = 0;
            for (int c = 0; c < d; c++) {
                int diff = point->coords[c] - centroid->coords[c];
                dist += diff * diff;
            }
            if (dist < minDist) {
                min = j;
                minDist = dist;
            }
        }
        linked_cluster_t* newCluster = malloc_impl(sizeof(linked_cluster_t));
        newCluster->point = malloc_impl(sizeof(point_t));
        newCluster->point->dim = d;
        newCluster->point->coords = malloc_impl(sizeof(int64_t) * d);
        memcpy(newCluster->point->coords, point->coords, sizeof(int64_t) * d);
        newCluster->next = clusters->clusters[min];
        newCluster->length = clusters->clusters[min]->length + 1;
        clusters->clusters[min] = newCluster;
    }
    return clusters;
}


points_array_t* computeCentroids(clusters_array_t* clusters, int k, int d) {
    points_array_t* centroids = malloc_impl(sizeof(points_array_t));
    centroids->length = k;
    centroids->points = malloc_impl(sizeof(point_t) * k);
    for (int i = 0; i < k; i++) {
        point_t* centroid = malloc_impl(sizeof(point_t));
        centroids->points[i] = centroid;
        linked_cluster_t* cluster = clusters->clusters[i];
        centroid->dim = d;
        centroid->coords = malloc_impl(sizeof(int64_t) * d);
        memset(centroid->coords, 0, sizeof(int64_t) * d);
        for (linked_cluster_t* current = cluster; current->length > 0; current = current->next) {
            point_t* point = current->point;
            for (int c = 0; c < d; c++) {
                centroid->coords[c] += point->coords[c];
            }
        }
        for (int c = 0; c < d; c++) {
            centroid->coords[c] /= cluster->length;
        }
    }
    return centroids;
}


void freePoints(points_array_t* points) {
    for (int i = 0; i < points->length; i++) {
        free_impl(points->points[i]->coords);
        free_impl(points->points[i]);
    }
    free_impl(points->points);
    free_impl(points);
}


void freeClusters(clusters_array_t* clusters) {
    for (int i = 0; i < clusters->length; i++) {
        linked_cluster_t* current = clusters->clusters[i];
        while (current->length > 0) {
            linked_cluster_t* next = current->next;
            free_impl(current->point->coords);
            free_impl(current->point);
            free_impl(current);
            current = next;
        }
        free_impl(current);
    }
    free_impl(clusters->clusters);
    free_impl(clusters);
}


points_array_t* kmeans(points_array_t* points, uint64_t n, int k, int d) {
    // Init
    points_array_t* centroids = malloc_impl(sizeof(points_array_t));
    centroids->length = k;
    centroids->points = malloc_impl(sizeof(point_t) * k);
    for (int i = 0; i < k; i++) {
        centroids->points[i] = malloc_impl(sizeof(point_t));
        centroids->points[i]->coords = malloc_impl(sizeof(int64_t) * d);
        centroids->points[i]->dim = d;
    }
    for (int i = 0; i < k; i++) {
        for (int c = 0; c < d; c++) {
            centroids->points[i]->coords[c] = points->points[i]->coords[c];
        }
    }
    clusters_array_t* clusters = computeClusters(points, n, centroids, k, d);

    // 10 iterations
    for (int i = 0; i < 10; i++) {
        clusters_array_t* newClusters = computeClusters(points, n, centroids, k, d);
        freeClusters(clusters);
        clusters = newClusters;
        points_array_t* newCentroids = computeCentroids(clusters, k, d);
        freePoints(centroids);
        centroids = newCentroids;
    }
    freeClusters(clusters);

    return centroids;
}