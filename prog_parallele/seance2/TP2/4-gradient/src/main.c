#include <stdio.h>
#include <stdlib.h>

#include "image_lib.h"


void test_image_gradient(const char *fname, int nb_threads)
{
  /* Allocate image structure for input image */
  image_t *img = image_new_open(fname);
  /* Allocate image structure for output (gradient) image */
  image_t *out = image_new(img->width-2, img->height-2, IMAGE_RGB_888);
  /* Compute the gradient */
  image_gradient(img, out, nb_threads);
  /* Save gradient image */
  image_save_binary(out, "out/gradient.bin.ppm");
  /* Clean up heap */
  image_delete(img);
  image_delete(out);
}


int main(int argc, char **argv)
{
  // Nombre de threads                                                                      
  int nb_threads;

  // Si aucun argument n'est passé
  if (argv[1] == NULL)
  {
      perror("No arguments passed when launching the program \n Default number of thread will be : 10 \n");
      nb_threads = 10;
  }
  else
  {
      // On récupère l'argument du terminal et on le converti en int
      nb_threads = atoi(argv[1]);
  }
  printf("Nombre de threads = %d\n", nb_threads);

  printf("Started.\n");

  if (argc >= 3) 
  {
    test_image_gradient(argv[2], nb_threads);
  }
  else
  {
    test_image_gradient("img/Brittany.pgm", nb_threads);  
  }
    
  printf("End.\n");
  return 0;
}