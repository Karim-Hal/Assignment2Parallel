#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <pthread.h>

typedef struct {
    int *array;
    size_t size;
} Bucket;

Bucket *allocate_buckets(int n_arrays);
int get_bucket_index(int value, int max, int n_buckets);
Bucket add_bucket_value(Bucket array, int value);
void insertion_sort(int *array, size_t size);
void combine_buckets(int *sort_array, Bucket *buckets, int n_buckets);
void free_buckets(Bucket *buckets, int n_buckets);
int *bucket_sort_openmp(int *array, size_t size, int max_value, int n_threads);

int main() {
    int array[1000];
    for (int i = 0; i < 1000; i++){
    array[i] = rand() % (1000+1);
    }
    size_t size = sizeof(array) / sizeof(array[0]); 
    int max_value = 0; 
    for (size_t i = 0; i < size; ++i) { 
        if (array[i] > max_value){ 
            max_value = array[i];
        } 
    } 
   
   
    int n_threads = 8; 
    
    
    
    int *sorted_array = bucket_sort_openmp(array, size, max_value, n_threads);
    
    printf("\n");

    return 0;
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
        return (int) (value) / (max / n_buckets); 
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

void insertion_sort(int *array, size_t size) {
    for (size_t i = 1; i < size; ++i) {
        int key = array[i];
        int j = i - 1;
        while (j >= 0 && array[j] > key) {
            array[j + 1] = array[j];
            --j;
        }
        array[j + 1] = key;
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

int *bucket_sort_openmp(int *array, size_t size, int max_value, int n_threads) {
 long double start, final;
 start = omp_get_wtime(); 
    Bucket *buckets = NULL;
    int *sort_array = NULL;
 



    buckets = allocate_buckets(n_threads); 
    sort_array = malloc(size * sizeof(int)); 

    for(size_t i = 0; i < size; i++) {
      
        int index = get_bucket_index(array[i], max_value, n_threads); 
        buckets[index] = add_bucket_value(buckets[index], array[i]); 
    }

    omp_set_dynamic(0); 
    #pragma omp parallel num_threads(n_threads)
    {
        #pragma omp parallel for
        for(int i = 0; i < n_threads; i++)
            insertion_sort(buckets[i].array, buckets[i].size); 
    }

    combine_buckets(sort_array, buckets, n_threads); 
  final = omp_get_wtime() - start; 
   printf("Using %d thread(s): %Lf\n",n_threads, final); 
  

    free_buckets(buckets, n_threads); 
    return sort_array; 
}
