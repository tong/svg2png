#include <float.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

// #define MAX_WIDTH 1920
// #define MAX_HEIGHT 1080

int main(int argc, char **argv) {
    char* svg_file;
    char* png_file;
    float dpi = 96.0f;
    float w_max;
    float h_max;
    argc--; argv++;
    while (argc) {
        char *arg = *argv++;
        argc--;
        if (strcmp(arg, "-i") == 0) {
            svg_file = *argv++;
            argc--;
        } else if (strcmp(arg, "-o") == 0) {
            png_file = *argv++;
            argc--;
        } else if (strcmp(arg, "--max-width") == 0) {
            char *c = *argv++;
            char* p_end;
            w_max = (float)strtof(c, &p_end);
            argc--;
        } else if (strcmp(arg, "--max-height") == 0) {
            char *c = *argv++;
            char* p_end;
            h_max = (float)strtof(c, &p_end);
            argc--;
        } else if (strcmp(arg, "--dpi") == 0) {
            char *c = *argv++;
            dpi = (int)strtol(c, NULL, 0);
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
    //printf("svg=%s, png=%s, dpi=%f w_max=%f h_max=%f\n", svg_file, png_file, dpi, w_max, h_max);

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
    if(w_max || h_max) {
        float r_hori = w_max / w;
        float r_vert = h_max / h;
        float r_max = (r_hori < r_vert) ? r_vert : r_hori;
        if(r_max < 1.0) scale = r_max;
    }
    const int png_w = w * scale;
    const int png_h = h * scale;
    //printf("%f %i %i\n",scale, png_w, png_h);
	nsvgRasterize(rast, image, 0, 0, scale, img, w, h, w*4);
    stbi_write_png(png_file, png_w, png_h, 4, img, w*4);
    return 0;
error:
	nsvgDeleteRasterizer(rast);
	nsvgDelete(image);
	return 1;
}
