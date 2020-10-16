#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <omp.h>

#define N_MAX 10000000LL
#define PI 3.14159265

int sine_value = 0;

int main (int argc, char **argv)
{
    omp_lock_t lock1;
    omp_lock_t lock2;
    omp_init_lock(&lock1);
    omp_init_lock(&lock2);

    int phase = 0;
    int amplitude = 10000;
    float x = 0;
   
    omp_set_lock(&lock1);
    omp_set_lock(&lock2);

    int nb_write = 0;
    FILE *file = NULL;
    file = fopen ("sine.txt" , "w");
    if (file == NULL)
    {
        printf("ERROR while opening file\n");
    }

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            for (phase = 0; phase < N_MAX; ++phase)
            {
                x = 40 * 0.001 * phase; 
                sine_value = (int)(amplitude * sin(x));
                omp_unset_lock(&lock1);
                omp_set_lock(&lock2);
            }
        }
        
        #pragma omp section
        {
            for (nb_write = 0; nb_write < N_MAX; ++nb_write)
            {
                #pragma omp critical
                {
                    omp_set_lock(&lock1);
                    fprintf(file, "%d\t%d\n", nb_write, sine_value);
                    omp_unset_lock(&lock2);
                }   
            }
        }
    }

    fclose(file);
    return (0);
}
