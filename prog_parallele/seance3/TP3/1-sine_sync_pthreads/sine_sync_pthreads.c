#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <semaphore.h>

#define N_MAX 10000000LL
#define PI 3.14159265

int sine_value = 0;
static pthread_mutex_t mutex1;
static pthread_mutex_t mutex2;

void *sine_producer (void *thread_arg)
{
    int phase = 0;
    int amplitude = 10000;
    float x = 0;

    for (phase = 0; phase < N_MAX; ++phase)
    {
        // La première fois le thread 1 calcule sine
        x = 40 * 0.001 * phase;
        sine_value = (int)(amplitude * sin(x));
        int ret1 = pthread_mutex_unlock(&mutex1);
        // Le thread 2 doit attendre que le thread 2 unlock le mutex 2
        int ret2 = pthread_mutex_lock (&mutex2);
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
        // Le thread 2 doit attendre que le thread 1 unlock le mutex1
        int ret1 = pthread_mutex_lock(&mutex1);
        fprintf(file, "%d\t%d\n", nb_write, sine_value); 
        int ret2 = pthread_mutex_unlock (&mutex2);
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
    
    pthread_create(&my_threads[0], NULL, sine_writer, NULL);
    pthread_create(&my_threads[1], NULL, sine_producer, NULL);

    // On lock pour forcer le programme à commencer par le thread 1
    int ret1 = pthread_mutex_lock(&mutex1);
    int ret2 = pthread_mutex_lock(&mutex2);
    
    for (int i = 0; i < n_threads; i++)
    {
        rc = pthread_join(my_threads[i], &thread_return);
        if (rc < 0)
            printf("pthread_join ERROR !!! (%d)\n", rc);
    }

    // On unlock pour destroy sans problèmes
    ret1 = pthread_mutex_unlock(&mutex1);
    ret2 = pthread_mutex_unlock(&mutex2);

    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    free (my_threads);

    return (0);
}
