#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define X_SIZE 4096
#define Y_SIZE 4096
#define IT_MAX 255

// // Prototypes
// void compute_set ( char *raster, double cx, double cy);
// long compute_point (double zx, double zy, double cx, double cy);
// int read_seed (double *cx, double *cy);
// void write_pgm( char *raster, char *name);

// int main (int argc, char **argv)
// {
//    char *raster;
//     char name [256];
//     double cx, cy;
//     raster = (char *) malloc (Y_SIZE * X_SIZE);
//     while (read_seed(&cx, &cy))
//     {
//         compute_set (raster, cx, cy);
//         sprintf (name, "julia_%f_%f.pgm", cx, cy);
//         write_pgm(raster, name);
//     }
//     free(raster);
//     return 0;
// }

// void compute_set(char *raster, double cx, double cy)
// {
//     long x, y;
//     double zx, zy;
//     for (y = 0; y < Y_SIZE; y++)
//     {
//         zy = 4.0 * (double) y / (double) (Y_SIZE - 1) - 2.0;
//         for (x = 0; x < X_SIZE; x++)
//         {
//             zx = 4.0 * (double) x / (double) (X_SIZE - 1) - 2.0;
//             raster[y * X_SIZE + x ] = compute_point ( zx, zy, cx, cy);
//         }
//     }
// }

// long compute_point (double zx, double zy, double cx, double cy)
// {
//     double zx_temp, zy_temp;
//     long it = 0;
//     while ((it < IT_MAX) && ((zx * zx) + (zy * zy) < 4.0))
//     {
//         zx_temp = zx * zx - zy * zy + cx;
//         zy_temp = 2 * zx * zy + cy;
//         zx = zx_temp;
//         zy = zy_temp;
//         it++;
//     }
//     return it;
// }

// int read_seed (double *cx, double *cy)
// {
//     if (scanf ("%lf %lf\n", cx, cy) == EOF)
//     {
//         return 0;
//     }
//     else
//     {
//         return 1;
//     }
// }

// void write_pgm( char *raster, char *name)
// {
//     FILE *fp;
//     fp = fopen (name, "wb");
//     fprintf (fp , "P5 %d %d %d\n", X_SIZE, Y_SIZE, IT_MAX);
//     fwrite (raster , 1 , X_SIZE * Y_SIZE, fp);
//     fclose (fp);
// }

// Prototypes
void* compute_set (void* p_args);
long compute_point (double zx, double zy, double cx, double cy);
int read_seed (double *cx, double *cy);
void write_pgm( char *raster, char *name);

typedef struct{
    char* t_raster;
    double t_cx;
    double t_cy;
    int t_min;
    int t_max;
    int t_id;
} arg;


int main (int argc, char **argv)
{   
    int nb_threads;

    if (argv[1] == NULL)
    {
        perror("No arguments passed when launching the program \n Default number of thread will be : 10 \n");
        nb_threads = 10;
    }else
    {
        nb_threads = atoi(argv[1]);
    }
    printf("Nombre de threads = %d\n", nb_threads);
    
    char name [256];
    double cx, cy;

    char *raster;
    raster = (char *) malloc (Y_SIZE * X_SIZE);

    // Tableaux de threads de la taille de l'argument récupéré en paramètres
    pthread_t threads[nb_threads];
    // Tableaux de structures d'arguments pour stocker les différentes arguments des threads
    arg tab_arg[nb_threads];
    
    // On divise le nombre max par le nombre de threads et on cast en int pour récupérer l'entier
    long long int res = Y_SIZE / nb_threads;
    // On récupère le reste de la division grâce à la fonction modulo
    long long int reste = Y_SIZE % nb_threads;

    while (read_seed(&cx, &cy))
    {
        // On parcourt le tableau de threads
        for (int i=0; i< nb_threads; i++)
        {
            // On initialise les arguments
            tab_arg[i].t_raster = raster;
            tab_arg[i].t_cx = cx;
            tab_arg[i].t_cy = cy;
            tab_arg[i].t_id = i;
            tab_arg[i].t_min = i*res;

            if(i == nb_threads-1)
            {
                tab_arg[i].t_max = tab_arg[i].t_min + res + reste;
            }
            else
            {
                tab_arg[i].t_max = tab_arg[i].t_min + res;
            }

            // On créé le thread avec pour paramètre un pointeur d'arguments
            int ret = pthread_create(&threads[i], NULL, compute_set, &tab_arg[i]);
            if(ret != 0) perror("Erreur creation de thread !\n");	
        }

        // On parcourt le tableau de threads
        for(int i = 0; i < nb_threads; i++)
        {
          // Pour chaque thread du tableau on attend qu'il soit fini
          pthread_join(threads[i], NULL);
        }

        //compute_set (raster, cx, cy);
        sprintf (name, "julia_%f_%f.pgm", cx, cy);
        write_pgm(raster, name);   
    }
    free(raster);
    return 0;
}

void* compute_set(void* p_args)
{
    // On récupère le pointeur null d'arguments que l'on cast en pointeur de structure arg
    arg *loc_args = (arg*) p_args;
    long x, y;
    double zx, zy;
    char* raster = loc_args->t_raster;
    double cx = loc_args->t_cx;
    double cy = loc_args->t_cy;
    int min = loc_args->t_min;
    int max = loc_args->t_max;

    for (y = min; y < max; y++)
    {
        zy = 4.0 * (double) y / (double) (Y_SIZE - 1) - 2.0;
        for (x = 0; x < X_SIZE; x++)
        {
            zx = 4.0 * (double) x / (double) (X_SIZE- 1) - 2.0;
            raster[y * X_SIZE + x ] = compute_point ( zx, zy, cx, cy);
        }
    }
    return NULL;
}

long compute_point (double zx, double zy, double cx, double cy)
{
    double zx_temp, zy_temp;
    long it = 0;
    while ((it < IT_MAX) && ((zx * zx) + (zy * zy) < 4.0))
    {
        zx_temp = zx * zx - zy * zy + cx;
        zy_temp = 2 * zx * zy + cy;
        zx = zx_temp;
        zy = zy_temp;
        it++;
    }
    return it;
}

int read_seed (double *cx, double *cy)
{
    if (scanf ("%lf %lf\n", cx, cy) == EOF)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void write_pgm( char *raster, char *name)
{
    FILE *fp;
    fp = fopen (name, "wb");
    fprintf (fp , "P5 %d %d %d\n", X_SIZE, Y_SIZE, IT_MAX);
    fwrite (raster , 1 , X_SIZE * Y_SIZE, fp);
    fclose (fp);
}
