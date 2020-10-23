#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <omp.h>

#define N_MAX 10000000LL
#define PI 3.14159265

int sine_value = 0;

int main(int argc, char **argv)
{
    omp_lock_t l1;
    omp_lock_t l2;
    omp_lock_t l3;
    omp_lock_t l4;
    omp_lock_t l5;
    omp_lock_t l6;

    omp_init_lock(&l1);
    omp_init_lock(&l2);
    omp_init_lock(&l3);
    omp_init_lock(&l4);
    omp_init_lock(&l5);
    omp_init_lock(&l6);

    omp_set_lock(&l1);
    omp_set_lock(&l2);
    omp_set_lock(&l3);
    omp_set_lock(&l4);
    omp_set_lock(&l5);
    omp_set_lock(&l6);

#pragma omp parallel for
        for (int i = 0; i < 4; i++)
        {
            int t_id = omp_get_thread_num();
            // printf("t_id = %d\n", t_id);
            if (i == 3)
            {
                int phase = 0;
                int amplitude = 10000;
                float x = 0;
            
                for (phase = 0; phase < N_MAX; ++phase)
                {
                    x = 40 * 0.001 * phase;
                    sine_value = (int)(amplitude * sin(x));

                    omp_unset_lock(&l1);
                    omp_unset_lock(&l2);
                    omp_unset_lock(&l3);

                    omp_set_lock(&l4);
                    omp_set_lock(&l5);
                    omp_set_lock(&l6);
                }
            }
            else
            {
                // printf("t_ids in writer : %d\n", t_id);
                int nb_write = 0;
                FILE *file = NULL;
                char filename[20] = {'\0'};
                
                sprintf(filename, "sine_%d.txt", t_id);
                file = fopen (filename , "w");
                if (file == NULL)
                {
                    printf("ERROR while opening file\n");
                }
                
                for (nb_write = 0; nb_write < N_MAX; ++nb_write)
                {
                    if(t_id == 0)
                    {
                        omp_set_lock(&l1);
                    }
                    else if(t_id == 1)
                    {
                        omp_set_lock(&l2);
                    }
                    else if(t_id == 2)
                    {
                        omp_set_lock(&l3);
                    }

                    fprintf(file, "%d\t%d\n", nb_write, sine_value);

                    if(t_id == 0)
                    {
                        omp_unset_lock(&l4);
                    }
                    else if(t_id == 1)
                    {
                        omp_unset_lock(&l5);
                    }
                    else if(t_id == 2)
                    {
                        omp_unset_lock(&l6);
                    }
                }
                fclose(file);
            }
        }
        return (0);
}



    //     int n_threads;
    // int rc;
    // pthread_t *my_threads;
    // thread_args_t *my_args;
    // void *thread_return;
    
    // n_threads = 4;
    
    // my_threads = calloc(n_threads, sizeof(pthread_t));
    // my_args = calloc(n_threads, sizeof(thread_args_t));
    
    // for (int i = 0; i < n_threads; i++)
    // {
    //     my_args[i].thread_id = i;
    //     if (i == (n_threads - 1))
    //         pthread_create(&my_threads[i], NULL, sine_producer, (void *)&my_args[i]);
    //     else
    //         pthread_create(&my_threads[0], NULL, sine_writers, (void *)&my_args[i]);
    // }
    
    // for (int i = 0; i < n_threads; i++)
    // {
    //     rc = pthread_join(my_threads[i], &thread_return);
    //     if (rc < 0)
    //         printf("pthread_join ERROR !!! (%d)\n", rc);
    // }
    // free (my_threads);