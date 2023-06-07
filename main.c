#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

int main(int argc, char **argv) {
    char * svg_file, * png_file;
    int dpi = 96, w_max = 3840, h_max = 2160;
    //float scale = 1.0;
    argc--; argv++;
    while (argc) {
        char*arg = *argv++;
        argc--;
        if (strcmp(arg, "-i") == 0) {
            svg_file = *argv++;
            argc--;
        } else if (strcmp(arg, "-o") == 0) {
            png_file = *argv++;
            argc--;
        } else if (strcmp(arg, "--max-width") == 0) {
            char * c = *argv++;
            w_max = (int)strtol(c, NULL, 0);
            argc--;
        } else if (strcmp(arg, "--max-height") == 0) {
            h_max = (int)strtol(*argv++, NULL, 0);
            argc--;
        } else if (strcmp(arg, "--dpi") == 0) {
            dpi = (int)strtol(*argv++, NULL, 0);
            argc--;
        } else if (strcmp(arg, "--help") == 0) {
            printf("Usage: svg2png -i <file.svg> -o <file.png> [options]\n\n"
                    "Options:\n"
                    "  --max-width <int> Max width of generated image\n"
                    "  --max-height <int> Max width of generated image\n"
                    "  --dpi <int> Dots per inch\n");
            return 0;
        } else {
            printf("Invalid arguments\n");
            return 1;
        }
    }
    if(!png_file) {
        const int len = strlen(svg_file);
        png_file = malloc(len+1);
        strncpy(png_file, svg_file, len-3);
        strcat(png_file, "png");
    }
    //printf("svg=%s, png=%s, dpi=%i w_max=%i h_max=%i\n", svg_file, png_file, dpi, w_max, h_max);
	NSVGimage *image = NULL;
	NSVGrasterizer *rast = NULL;
	unsigned char* img = NULL;
	int w, h;
	image = nsvgParseFromFile(svg_file, "px", dpi);
	if (image == NULL) {
		printf("Could not open svg image.\n");
		goto error;
	}
	w = (int)image->width;
	h = (int)image->height;
  //   if(w > MAX_WIDTH || h > MAX_WIDTH) {
		// printf("Max image size exceeded.\n");
  //       goto error;
  //   }
	rast = nsvgCreateRasterizer();
	if (rast == NULL) {
		printf("Could not init svg rasterizer.\n");
		goto error;
	}
	img = malloc(w*h*4);
	if (img == NULL) {
		printf("Could not alloc image buffer.\n");
		goto error;
        return 1;
	}
    float scale = 1.0;
    if(w_max != 0 || h_max == 0) {
        float rh = (float) w_max / w;
        float rv = (float) h_max / h;
        float r = (rh > rv) ? rv : rh;
        //printf("%f %f %f\n", rh, rv, r);
        if(r < 1.0) scale = r;
    }
    float png_w = roundf(w * scale);
    float png_h = roundf(h * scale);
    //printf("%i %i %ld %i %i\n", w, h, (long)scale, (int)png_w, (int)png_h);
	nsvgRasterize(rast, image, 0, 0, scale, img, w, h, w*4);
    stbi_write_png(png_file, png_w, png_h, 4, img, w*4);
    return 0;
error:
	nsvgDeleteRasterizer(rast);
	nsvgDelete(image);
	return 1;
}
