
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

sem_t sem;
struct args{
 float* arr;
 int w;
 int h;
 int row;
 int col;
 int* found_x;
 int* found_y;
 float* max;
};


int* generateNeighbours(int w, int h, int row, int col){

    int* neighbours = malloc(sizeof(int) * 9);

    for(int i = 0; i < 9; i++){
        neighbours[i] = -1;
    }

    int pos = 0;

    for(int r = row - 1; r <= row + 1; r++){
        for(int c = col - 1; c <= col + 1; c++){
            if(r < 0 || r >= h) continue;
            if(c < 0 || c >= w) continue;
            int index = r*w + c;
            neighbours[pos] = index;
            pos++;
        }
    }
    return neighbours;
}



void* calculate(void* a){
    struct args* args = (struct args*) a;
    int* neighbours = generateNeighbours(args->w,args->h,args->row,args->col);
    float sum = 0;

    for(int i = 0; i < 9; i++){
        if(neighbours[i] != -1){
            sum += args->arr[neighbours[i]];
        }
    }
    
    free(neighbours);
    sem_wait(&sem);
    if(sum > *(args->max)){
        *(args->max) = sum;
        *(args->found_x) = args->col;
        *(args->found_y) = args->row;
    }
    sem_post(&sem);
    return NULL;
}

void get_hma (float * array, int w, int h, int * found_x, int * found_y) {
    float max = -1;
    struct args args = {array, w, h, 0, 0, found_x, found_y, &max};
    for(args.row = 0; args.row < h; args.row++){
        for(args.col = 0; args.col < w; args.col++){
            calculate(&args);
        }
    }
}

void get_hma_parallel(int nthreads, float * array, int w, int h, int * found_x, int * found_y) {
    pthread_t* threads = malloc(sizeof(pthread_t) * nthreads);
    float max = -1;
    struct args args = {array, w, h, 0, 0, found_x, found_y, &max};
    int p = 0;
    for(args.row  = 0; args.row  < h; args.row ++){
        for(args.col = 0; args.col < w; args.col++){
            pthread_create(&threads[p++], NULL, &calculate, &args);
        }
    }

    for(int i = 0; i < p; i++){
        pthread_join(threads[i], NULL);
    }

    free(threads);

}

int student_main(int argc, char **argv)
{
    int width = 3;
    int height = 3;
    int* found_x = malloc(sizeof(int));
    int* found_y = malloc(sizeof(int));
    float grid[9] = {-100,0,0,0,5,6,0,8,9};

    int nthreads = 9;
    sem_init(&sem, 0, 1);
    get_hma(grid, width, height, found_x, found_y);

    printf("Serial: \nFound_X is %d \nFound_Y is %d\n", *found_x, *found_y);

    *found_x = -1;
    *found_y = -1;

    get_hma_parallel(nthreads, grid, width, height, found_x, found_y);
    
    printf("Parallel: \nFound_X is %d \nFound_Y is %d\n", *found_x, *found_y);
    free(found_x);
    free(found_y);
    return 0;
}


/*
 * DO NOT MODIFY THIS MAIN FUNCTION
 */
int main(int argc, char **argv) // #151479
{
    return student_main(argc, argv);
}


