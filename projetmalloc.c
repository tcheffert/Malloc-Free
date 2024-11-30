#include <stdint.h>
#include <stdio.h>
#include <time.h>

//#include "bad_kmeans.h"
//#include "bad_kmeans.c"

uint8_t MY_HEAP[64000];

typedef struct metadonnee
{
    //size_t size;             // taille de la zone
    int free;               // 1 si libre , 0 sinon
    struct metadonnee *next; // point vers le prochain bloc
    struct metadonnee *prev; // point vers le bloc précédent
} metadonnee;

void init()
{
    // To do
    metadonnee *init = (metadonnee *)MY_HEAP;
    //init->size = 64000 - sizeof(metadonnee);
    init->free = 1;
    init->next = NULL;
    init->prev = NULL;
    /*printf("size metadonnee %d \n",sizeof(metadonnee));
    printf("size my heap %d \n",sizeof(MY_HEAP));
    printf("size init %d \n",sizeof(init));
    printf("size init-> next %d \n",sizeof(init->next));
    printf("size of init->free %d \n",sizeof(init->free));
    printf("size of init->prev %d \n",sizeof(init->prev));*/
    return;
}

void *my_malloc(size_t size)
{
    if (size == 0 || size > sizeof(MY_HEAP) - sizeof(metadonnee)) // taille trop grande
    {
        return NULL; // Size est pas valide
    }

    // To do
    // first have to go through all blocks in the heap to find the next free block
    metadonnee *start = (metadonnee *)MY_HEAP;                               // Problème de casting: start c'est un meta donné pas un int non ?
    size_t blocSize = (uint8_t *) start->next - (uint8_t *) start - sizeof(metadonnee); // Taille du bloc
    while ((blocSize < size || start->free == 0) && start->next != NULL) // on itère si le bloc est trop petit OU pas libre ET stop s'il y a pas de next
    {                                                                        // itere jusqu'a trouver un bloc libre de taille suffisante
        start = start->next;
        blocSize = (uint8_t *) start->next - (uint8_t *) start - sizeof(metadonnee);
    }
    // On exit la boucle car on a trouvé un spot où placer le nouveau bloc

    // FIT parfait (size = size dispo memory)
    if (blocSize== size && start->free == 1)
    {
        start->free = 0;
        void *result = (void *)((uint8_t *) start + sizeof(metadonnee));
        return result; // Faut retourner un void* donc faut cast je crois
    }

    else if (blocSize > size + sizeof(metadonnee))
    {// split le nouveau bloc atteint entre celui a allouer et celui restant
        start->free = 0; // C'est pas "libre" ici ? Fin il y a pas de champs "free" dans start donc je suppose que tu voulais dire "libre"
        //start->size = size;

        metadonnee *newBloc = (metadonnee *)((uint8_t *)start + sizeof(metadonnee) + size);
        //newBloc->size = start->size - size - sizeof(metadonnee); // Size du nouveau bloc (size start dans mem - size donné = espace restant)
        newBloc->free = 1;                                      // Ce nouveau bloc est libre

        // Update les links du nouveau bloc dans la list (memoire)
        newBloc->next = start->next;
        newBloc->prev = start;
        start->next = newBloc;

        // return start + sizeof(metadonnee); // verif comment mettre ca dans la linked list
        void *result = (void *)((uint8_t *)start + sizeof(metadonnee));
        return result; // Faut retourner un void* donc faut cast je crois
    }

    // Sinon, la mémoire full
    return NULL; // plus de bloc de taille suffisante disponible
}

void my_free(void *pointer)
{
    // Check if the ptr is valid
    uint8_t *ptr8 = (uint8_t *)pointer;
    uint8_t *heap = (uint8_t *)MY_HEAP;
    if (ptr8 != NULL && ptr8 >= heap && ptr8 < (heap + sizeof(MY_HEAP)))
    {
        metadonnee *bloc = (metadonnee *)(ptr8- sizeof(metadonnee));
        bloc->free = 1;

        if (bloc->prev != NULL && bloc->prev->free == 1) // previous bloc is free
        {
            //MERGE
            bloc->prev->next = bloc->next;
            if (bloc->next != NULL){
                bloc->next->prev = bloc->prev; // add condition in case there is no next
            }

            // on switch le pointeur sur le bloc précédent pour ne pas avoir de soucis si le prochain bloc est free et
            // qu'on veut le merge
            bloc = bloc->prev;
        }

        if (bloc->next != NULL && bloc->next->free == 1) // next bloc is free
        {
            //MERGE
            if( bloc->next->next != NULL){
                bloc->next->next->prev = bloc;
            }
            bloc->next = bloc->next->next;
        }
        
    }

    // To do
    // recup la metadonnee , supprimer le bloc , fusionner les blocs vides
    return;
}


/*
void*(*malloc_impl)(size_t);
void(*free_impl)(void*);

int main() {
    init();
    
    srand(314);
    points_array_t* points = malloc(sizeof(points_array_t));
    points->length = 100;
    points->points = malloc(sizeof(point_t*) * 100);
    for (int i = 0; i < 100; i++) {
        points->points[i] = malloc(sizeof(point_t));
        points->points[i]->dim = 3;
        points->points[i]->coords = malloc(sizeof(int64_t) * 3);
        points->points[i]->coords[0] = rand();
        points->points[i]->coords[1] = rand();
        points->points[i]->coords[2] = rand();
    }

    // Reference
    malloc_impl = malloc;
    free_impl = free;
    points_array_t* ref = kmeans(points, 100, 5, 3);
    printf("Centroids:\n");
    for (int i = 0; i < ref->length; i++) {
        printf("%ld %ld %ld\n", ref->points[i]->coords[0], ref->points[i]->coords[1], ref->points[i]->coords[2]);
    }

    // With standard malloc
    malloc_impl = malloc;
    free_impl = free;
    clock_t start = clock();
    for (int i = 0; i < 1000; i++) {
        points_array_t* centroids = kmeans(points, 100, 5, 3);
        if (centroids->length != ref->length) {
            printf("Error: length %ld != %ld\n", centroids->length, ref->length);
        }
        for (int i = 0; i < centroids->length; i++) {
            if (centroids->points[i]->dim != ref->points[i]->dim) {
                printf("Error: dim %d != %d\n", centroids->points[i]->dim, ref->points[i]->dim);
            }
            for (int j = 0; j < centroids->points[i]->dim; j++) {
                if (centroids->points[i]->coords[j] != ref->points[i]->coords[j]) {
                    printf("Error: coord %ld != %ld\n", centroids->points[i]->coords[j], ref->points[i]->coords[j]);
                }
            }
        }
        freePoints(centroids);
    }
    clock_t end = clock();
    printf("Standard malloc time: %f\n", (double)(end - start) / CLOCKS_PER_SEC);

    // With custom malloc
    malloc_impl = my_malloc;
    free_impl = my_free;
    start = clock();
    for (int i = 0; i < 1000; i++) {
        points_array_t* centroids = kmeans(points, 100, 5, 3);
        if (centroids->length != ref->length) {
            printf("Error: length %ld != %ld\n", centroids->length, ref->length);
        }
        for (int i = 0; i < centroids->length; i++) {
            if (centroids->points[i]->dim != ref->points[i]->dim) {
                printf("Error: dim %d != %d\n", centroids->points[i]->dim, ref->points[i]->dim);
            }
            for (int j = 0; j < centroids->points[i]->dim; j++) {
                if (centroids->points[i]->coords[j] != ref->points[i]->coords[j]) {
                    printf("Error: coord %ld != %ld\n", centroids->points[i]->coords[j], ref->points[i]->coords[j]);
                }
            }
        }
        freePoints(centroids);
    }
    end = clock();
    printf("Custom malloc time: %f\n", (double)(end - start) / CLOCKS_PER_SEC);
    
    free_impl = free;
    freePoints(ref);
    freePoints(points);
}*/

int main(){
    init();
    void *ptr = my_malloc(100);
    void *ptr2 = my_malloc(100);
    void *ptr3 = my_malloc(100);
    void *ptr4 = my_malloc(100);
    void *ptr5 = my_malloc(100);
    my_free(ptr);
    my_free(ptr2);
    my_free(ptr3);
    my_free(ptr4);
    my_free(ptr5);
    return 0;
}