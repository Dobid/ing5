#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <omp.h>

#define N_MAX 10000000LL
#define PI 3.14159265

// typedef struct {
//   int thread_id;
// } thread_args_t;


// void *sine_producers (void *thread_arg)
// {
//     thread_args_t *my_args = (thread_args_t *)(thread_arg);
//     int phase = 0;
//     int amplitude = 10000;
//     float x = 0;
    
//     for (phase = 0; phase < N_MAX; ++phase)
//     {
//         x = 40 * 0.001 * phase + my_args->thread_id;
//         sine_value[my_args->thread_id%3] = (int)(amplitude * sin(x));
//     }
    
//     return NULL;
// }

// void *sine_writers (void *thread_arg)
// {
//     thread_args_t *my_args = (thread_args_t *)(thread_arg);
//     int nb_write = 0;
//     FILE *file = NULL;
//     char filename[20] = {'\0'};
    
//     sprintf(filename, "sine_%d.txt", my_args->thread_id);
//     file = fopen (filename , "w");
//     if (file == NULL)
//     {
//         printf("ERROR while opening file\n");
//     }
    
//     for (nb_write = 0; nb_write < N_MAX; ++nb_write)
//     {
//         fprintf(file, "%d\t%d\n", nb_write, sine_value[my_args->thread_id%3]);
//     }
    
//     fclose(file);
    
//     return NULL;
// }

int sine_value[3];

int main (int argc, char **argv)
{
    omp_lock_t l0;
    omp_lock_t l1;
    omp_lock_t l2;
    omp_lock_t l3;
    omp_lock_t l4;
    omp_lock_t l5;

    omp_init_lock(&l0);
    omp_init_lock(&l1);
    omp_init_lock(&l2);
    omp_init_lock(&l3);
    omp_init_lock(&l4);
    omp_init_lock(&l5);

    omp_unset_lock(&l0);
    omp_set_lock(&l1);
    omp_unset_lock(&l2);
    omp_set_lock(&l3);
    omp_unset_lock(&l4);
    omp_set_lock(&l5);

    int n_threads = 6;

#pragma omp parallel for 
    for (int i = 0; i < n_threads; i++)
    {
        int t_id = omp_get_thread_num();
        if (t_id%2 == 0)
        {
            int phase = 0;
            int amplitude = 10000;
            float x = 0;
    
            for (phase = 0; phase < N_MAX; ++phase)
            {
                if(t_id == 0)
                {
                    omp_set_lock(&l0);
                }
                if(t_id == 2)
                {
                    omp_set_lock(&l2);
                }
                if(t_id == 4)
                {
                    omp_set_lock(&l4);
                }

                x = 40 * 0.001 * phase + t_id;
                sine_value[t_id%3] = (int)(amplitude * sin(x));

                if(t_id == 0)
                {
                    omp_unset_lock(&l3);
                }
                if(t_id == 2)
                {
                    omp_unset_lock(&l5);
                }
                if(t_id == 4)
                {
                    omp_unset_lock(&l1);
                }
            }
        }
        else
        {
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
                if(t_id == 3)
                {
                    omp_set_lock(&l3);
                }
                if(t_id == 5)
                {
                    omp_set_lock(&l5);
                }
                if(t_id == 1)
                {
                    omp_set_lock(&l1);
                }

                fprintf(file, "%d\t%d\n", nb_write, sine_value[t_id%3]);

                if(t_id == 3)
                {
                    omp_unset_lock(&l0);
                }
                if(t_id == 5)
                {
                    omp_unset_lock(&l2);
                }
                if(t_id == 1)
                {
                    omp_unset_lock(&l4);
                }
            }
    
            fclose(file);
        }
    }
    return (0);
}