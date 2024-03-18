#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <pthread.h>
#include <math.h>

typedef struct {
    int *array;
    size_t size;
} Bucket;

Bucket *allocate_buckets(int n_arrays);
int get_bucket_index(int value, int max, int n_buckets);
Bucket add_bucket_value(Bucket array, int value);
void *insertion_sort(void *array);
void combine_buckets(int *sort_array, Bucket *buckets, int n_buckets);
void free_buckets(Bucket *buckets, int n_buckets);
int *bucket_sort_pthreads(int *array, size_t size, int max_value, int n_threads);

int main() {
    int array[20000];
    for (int i = 0; i < 20000; i++){
    array[i] = rand() % (20000+1);
    }
    size_t size = sizeof(array) / sizeof(array[0]); 
    int max_value = 0; 
    for (size_t i = 0; i < size; ++i) { 
        if (array[i] > max_value){ 
            max_value = array[i];
        } 
    } 
    int n_threads = 2; 
    int *sorted_array = bucket_sort_pthreads(array, size, max_value, n_threads);  
 
  

    return 0;
}


int *bucket_sort_pthreads(int *array, size_t size, int max_value, int n_threads){
  pthread_t threads[n_threads];
    Bucket *buckets = NULL;
    int *sort_array = NULL;
    long double start, final;

    start = omp_get_wtime();

    buckets = allocate_buckets(n_threads);

    sort_array = malloc(size * sizeof(int));

    for(size_t i = 0; i < size; i++){
        int index = get_bucket_index(array[i],max_value,n_threads);
        buckets[index] = add_bucket_value(buckets[index], array[i]);
       
    }

    for(int i = 0; i < n_threads; i++)
    pthread_create(&threads[i], NULL, (void *) insertion_sort, (void *) &buckets[i]);

    for(int i = 0; i < n_threads; i++){
        pthread_join(threads[i], NULL);
    }

    combine_buckets(sort_array, buckets, n_threads);

    final = omp_get_wtime() - start;

    printf("Using %d thread(s): %Lf\n", n_threads, final);

    free_buckets(buckets, n_threads);

    return sort_array;
}



Bucket *allocate_buckets(int n_arrays) {
    Bucket *arrays;
    arrays = (Bucket *) malloc(n_arrays * sizeof(Bucket)); 
    for(int i = 0; i < n_arrays; i++) {
        arrays[i].array = NULL; 
        arrays[i].size = 0; 
    }
    return arrays;
}

int get_bucket_index(int value, int max, int n_buckets) {
    if (value!=max){
   
      
      
        return (int)((value) / ((double)max / n_buckets)); 
    }
    else{
     
        return n_buckets-1;
    }
   
}

Bucket add_bucket_value(Bucket array, int value) {
    array.array = realloc(array.array, (array.size + 1) * sizeof(int)); 
    array.array[array.size] = value; 
    array.size++; 
    return array; 
}

void *insertion_sort(void *input) {
    Bucket *b = (Bucket *) input; 
    
    for (size_t i = 1; i < b->size; ++i) {
        int key = b->array[i];
        int j = i - 1;
        while (j >= 0 && b->array[j] > key) {
            b->array[j + 1] = b->array[j];
            --j;
        }
        b->array[j + 1] = key;
    }
}


void combine_buckets(int *sort_array, Bucket *buckets, int n_buckets) {
    int cont = 0;
    for(int i = 0; i < n_buckets; i++) {
        for(size_t j = 0; j < buckets[i].size; j++)
            sort_array[cont++] = buckets[i].array[j]; 
    }
}

void free_buckets(Bucket *buckets, int n_buckets) {
    for(int i = 0; i < n_buckets; i++)
        free(buckets[i].array); 
    free(buckets); 
}
