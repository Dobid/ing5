#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <semaphore.h>

#define N_MAX 4 //10000000LL
#define PI 3.14159265

sem_t sem1;
sem_t sem2;
pthread_mutex_t mux;

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
        printf("producer\n");
        x = 40 * 0.001 * phase;
        sine_value = (int)(amplitude * sin(x));

        int ret1 = sem_post(&sem1);
        printf("sempost 1\n");
        sem_getvalue(&sem1, &nb_jetons);
        printf("nb_jetons: %d\n", nb_jetons);
        ret1 = sem_post(&sem1);
        printf("sempost 2\n");
        sem_getvalue(&sem1, &nb_jetons);
        printf("nb_jetons: %d\n", nb_jetons);
        ret1 = sem_post(&sem1);
        printf("sempost 3\n");
        sem_getvalue(&sem1, &nb_jetons);
        printf("nb_jetons: %d\n", nb_jetons);

        int ret2 = sem_wait(&sem2);
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
    
    sprintf(filename, "sine_%d.txt", my_args->thread_id);
    file = fopen (filename , "w");
    if (file == NULL)
    {
        printf("ERROR while opening file\n");
    }
    
    for (nb_write = 0; nb_write < N_MAX; ++nb_write)
    {   
        int ret1 = sem_wait(&sem1);
        printf("id = %d\n", my_args->thread_id);
        fprintf(file, "%d\t%d\n", nb_write, sine_value);
        
        sem_getvalue(&sem1, &nb_jetons);

        if(nb_jetons == 0)
        {
            int ret2 = sem_post(&sem2);
            printf("sempost sem2\n");
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
    
    // Init semaphores à 0
    int ret1 = sem_init(&sem1, 0, 0);
    int ret2 = sem_init(&sem2, 0, 0);

    int ret3 = pthread_mutex_unlock(&mux);

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
    free (my_threads);

    return (0);
}