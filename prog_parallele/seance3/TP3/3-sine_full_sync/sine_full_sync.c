#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <semaphore.h>

#define N_MAX 10000000LL
#define PI 3.14159265

sem_t sem0;
sem_t sem1;
sem_t sem2;
sem_t sem3;
sem_t sem4;
sem_t sem5;

typedef struct {
  int thread_id;
} thread_args_t;

int sine_value[3];

void *sine_producers (void *thread_arg)
{
    thread_args_t *my_args = (thread_args_t *)(thread_arg);
    int phase = 0;
    int amplitude = 10000;
    float x = 0;
    
    for (phase = 0; phase < N_MAX; ++phase)
    {
        if(my_args->thread_id == 0)
        {
            sem_wait(&sem0);
        }
        if(my_args->thread_id == 2)
        {
            sem_wait(&sem2);
        }
        if(my_args->thread_id == 4)
        {
            sem_wait(&sem4);
        }

        x = 40 * 0.001 * phase + my_args->thread_id; // 0, 2 ou 4
        sine_value[my_args->thread_id%3] = (int)(amplitude * sin(x)); // 0%3 = 0, 2%3 = 2, 4%3 = 1 

        if(my_args->thread_id == 0)
        {
            sem_post(&sem3);
        }
        if(my_args->thread_id == 2)
        {
            sem_post(&sem5);
        }
        if(my_args->thread_id == 4)
        {
            sem_post(&sem1);
        }
    }
    
    return NULL;
}

void *sine_writers (void *thread_arg)
{
    thread_args_t *my_args = (thread_args_t *)(thread_arg);
    int nb_write = 0;
    FILE *file = NULL;
    char filename[20] = {'\0'};
    
    sprintf(filename, "sine_%d.txt", my_args->thread_id); // 1, 3 ou 5
    file = fopen (filename , "w");
    if (file == NULL)
    {
        printf("ERROR while opening file\n");
    }
    
    for (nb_write = 0; nb_write < N_MAX; ++nb_write)
    {
        if(my_args->thread_id == 3)
        {
            sem_wait(&sem3);
        }
        if(my_args->thread_id == 5)
        {
            sem_wait(&sem5);
        }
        if(my_args->thread_id == 1)
        {
            sem_wait(&sem1);
        }

        fprintf(file, "%d\t%d\n", nb_write, sine_value[my_args->thread_id%3]); // 1%3 = 1, 3%3 = 0, 5%3 = 2

        if(my_args->thread_id == 3)
        {
            sem_post(&sem0);
        }
        if(my_args->thread_id == 5)
        {
            sem_post(&sem2);
        }
        if(my_args->thread_id == 1)
        {
            sem_post(&sem4);
        }
    }
    
    fclose(file);
    
    return NULL;
}

int main (int argc, char **argv)
{
    int n_threads;
    int rc;
    pthread_t *my_threads;
    thread_args_t *my_args;
    void *thread_return;
    
    n_threads = 6;

    // Init semaphores à 0
    sem_init(&sem0, 0, 1);
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 1);
    sem_init(&sem3, 0, 0);
    sem_init(&sem4, 0, 1);
    sem_init(&sem5, 0, 0);
    
    my_threads = calloc(n_threads, sizeof(pthread_t));
    my_args = calloc(n_threads, sizeof(thread_args_t));
    
    for (int i = 0; i < n_threads; i++)
    {
        my_args[i].thread_id = i;
        if (i%2 == 0) // 0, 2, 4
            pthread_create(&my_threads[i], NULL, sine_producers, (void *)&my_args[i]);
        else // 1, 3, 5
            pthread_create(&my_threads[0], NULL, sine_writers, (void *)&my_args[i]);
    }
    
    for (int i = 0; i < n_threads; i++)
    {
        rc = pthread_join(my_threads[i], &thread_return);
        if (rc < 0)
            printf("pthread_join ERROR !!! (%d)\n", rc);
    }
    
    sem_destroy(&sem0);
    sem_destroy(&sem1);
    sem_destroy(&sem2);
    sem_destroy(&sem3);
    sem_destroy(&sem4);
    sem_destroy(&sem5);
    free (my_threads);

    return (0);
}