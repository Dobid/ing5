#include "image_gradient.h"
#include "utils.h"
#include "pixel.h"
#include <math.h>
#include <assert.h>
#include <omp.h>

static const float sobel_x[3][3] =
{
    {-.125, .000, .125},
    {-.250, .000, .250},
    {-.125, .000, .125}
};

static const float sobel_y[3][3] =
{
    {-.125, -.250, -.125},
    { .000,  .000,  .000},
    { .125,  .250,  .125}
};

void image_gradient(image_t *self, image_t *out, int nb_threads)
{
    /* input image should be a 8-bit grayscale image */
    assert(out->width >= self->width - 2);
    assert(out->height >= self->height - 2);
    assert(out->type == IMAGE_RGB_888);
    assert(self->type == IMAGE_GRAYSCALE_8);

    int x, y;
    uint8_t c;
    int kx, ky;
    float gx, gy, mag, angle;
    float h, s, v, abc;
    rgb_t rgb;

    // On indique le nombre dethreads a créer, ce nombre est récupéré dans les arg du main
    omp_set_num_threads(nb_threads);

    /* For all pixels of the input image */
    for (y = 0; y < self->height - 2; ++y)
    {
        // On parallélise la prochaine boucle for en appliquant ue reduction clause sur gx et gy qui sont shared
        #pragma omp parallel for private(x,mag,angle,h,s,v,rgb,ky,kx,c), reduction(+:gx,gy)
        for (x = 0; x < self->width - 2; ++x)
        {
            /* Compute gradient in x and y direction: */
            gx = 0;
            gy = 0;

            for (ky = 0; ky < 3; ++ky)
            {
                for (kx = 0; kx < 3; ++kx)
                {
                    c = self->getpixel(self, x + kx, y + ky).gs8;
                    gx += sobel_x[ky][kx] * c;
                    gy += sobel_y[ky][kx] * c;
                }
            }
            /* Compute the magnitude, and direction, of the gradient */
            mag = sqrt(gx * gx + gy * gy);
            angle = atan2(gy, gx) * 180./M_PI;
            
            /* Store as pixel intensity (magnitude) and color hue (angle) in output image */
            h = angle;
            s = 1.0;
            v = mag / 50.;
            rgb = rgb_from_hsv((hsv_t){.h = h, .s = s, .v = v});
            out->setpixel(out, x, y, (color_t){.rgb = rgb});
        }
    }

}