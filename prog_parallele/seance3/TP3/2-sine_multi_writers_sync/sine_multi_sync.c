#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <semaphore.h>

#define N_MAX 10000000LL
#define PI 3.14159265

sem_t sem1;
sem_t sem2;
sem_t sem3;
sem_t sem4;
sem_t sem5;
sem_t sem6;

typedef struct {
  int thread_id;
} thread_args_t;

int sine_value = 0;
int cpt = 0;

void *sine_producer (void *thread_arg)
{
    int phase = 0;
    int amplitude = 10000;
    float x = 0;
    int nb_jetons = 0;
    
    for (phase = 0; phase < N_MAX; ++phase)
    {
        // printf("producer\n");
        x = 40 * 0.001 * phase;
        sine_value = (int)(amplitude * sin(x));

        int ret = sem_post(&sem1);
        ret = sem_post(&sem2);
        ret = sem_post(&sem3);

        ret = sem_wait(&sem4);
        ret = sem_wait(&sem5);
        ret = sem_wait(&sem6);
    }

    return NULL;
}

void *sine_writers (void *thread_arg)
{
    thread_args_t *my_args = (thread_args_t *)(thread_arg);
    int nb_write = 0;
    FILE *file = NULL;
    char filename[20] = {'\0'};
    int nb_jetons = 0;
    int ret = 0;
    
    sprintf(filename, "sine_%d.txt", my_args->thread_id);
    file = fopen (filename , "w");
    if (file == NULL)
    {
        printf("ERROR while opening file\n");
    }
    
    for (nb_write = 0; nb_write < N_MAX; ++nb_write)
    {   
        if(my_args->thread_id == 0)
        {
            ret = sem_wait(&sem1);
        }
        else if(my_args->thread_id == 1)
        {
            ret = sem_wait(&sem2);
        }
        else if(my_args->thread_id == 2)
        {
            ret = sem_wait(&sem3);
        }
    
        // printf("id = %d\n", my_args->thread_id);
        fprintf(file, "%d\t%d\n", nb_write, sine_value);

        if(my_args->thread_id == 0)
        {
            ret = sem_post(&sem4);
        }
        else if(my_args->thread_id == 1)
        {
            ret = sem_post(&sem5);
        }
        else if(my_args->thread_id == 2)
        {
            ret = sem_post(&sem6);
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
    int ret = 0;

    // Init semaphores à 0
    ret = sem_init(&sem1, 0, 0);
    ret = sem_init(&sem2, 0, 0);
    ret = sem_init(&sem3, 0, 0);
    ret = sem_init(&sem4, 0, 0);
    ret = sem_init(&sem5, 0, 0);
    ret = sem_init(&sem6, 0, 0);

    n_threads = 4;
    
    my_threads = calloc(n_threads, sizeof(pthread_t));
    my_args = calloc(n_threads, sizeof(thread_args_t));
    
    for (int i = 0; i < n_threads; i++)
    {
        my_args[i].thread_id = i;
        if (i == (n_threads - 1))
            pthread_create(&my_threads[i], NULL, sine_producer, (void *)&my_args[i]);
        else
            pthread_create(&my_threads[0], NULL, sine_writers, (void *)&my_args[i]);
    }
    
    for (int i = 0; i < n_threads; i++)
    {
        rc = pthread_join(my_threads[i], &thread_return);
        if (rc < 0)
            printf("pthread_join ERROR !!! (%d)\n", rc);
    }
    
    sem_destroy(&sem1);
    sem_destroy(&sem2);
    sem_destroy(&sem3);
    sem_destroy(&sem4);
    sem_destroy(&sem5);
    sem_destroy(&sem6);
    free (my_threads);

    return (0);
}