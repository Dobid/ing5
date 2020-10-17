#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>

#define N_MAX 1000
#define PI 3.14159265

int sine_value = 0;
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

void *sine_producer(void *thread_arg)
{
    int phase = 0;
    int amplitude = 10000;
    float x = 0;
    
    for (phase = 0; phase < N_MAX; ++phase)
    {
        x = 40 * 0.001 * phase;
        sine_value = (int)(amplitude * sin(x));
        pthread_mutex_unlock(&mutex1);
        pthread_mutex_lock(&mutex2);
    }
    return NULL;
}

void *sine_writer (void *thread_arg)
{
    int nb_write = 0;
    FILE *file = NULL;
    file = fopen ("sine.txt" , "w");
    if (file == NULL)
    {
        printf("ERROR while opening file\n");
    }
    
    for (nb_write = 0; nb_write < N_MAX; ++nb_write)
    {
        pthread_mutex_lock(&mutex1);
        fprintf(file, "%d\t%d\n", nb_write, sine_value);
        pthread_mutex_unlock(&mutex2);
    }
    fclose(file);
    
    return NULL;
}

int main (int argc, char **argv)
{
    int n_threads;
    int rc;
    pthread_t *my_threads;
    void *thread_return;
    
    n_threads = 2;
    my_threads = calloc(n_threads, sizeof(pthread_t));

    if(pthread_mutex_init(&mutex1, NULL) != 0)
    {
        perror("Couldn't initialise mutex1\n");
    }
    if(pthread_mutex_init(&mutex2, NULL) != 0)
    {
        perror("Couldn't initialise mutex2\n");
    }
    
    pthread_create(&my_threads[0], NULL, sine_writer, NULL);
    pthread_create(&my_threads[1], NULL, sine_producer, NULL);

    for (int i = 0; i < n_threads; i++)
    {
        rc = pthread_join(my_threads[i], &thread_return);
        if (rc < 0)
            printf("pthread_join ERROR !!! (%d)\n", rc);
    }
    
    free (my_threads);
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);

    return (0);
}
