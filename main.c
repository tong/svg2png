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

#define MAX_WIDTH 1920
#define MAX_HEIGHT 1080

int main(int argc, char **argv) {
    char* svg_file;
    char* png_file;
    float dpi = 96.0f;
    argc--; argv++;
    while (argc) {
        char *arg = *argv++;
        argc--;
        //printf("%i %s\n", argc, arg);
        if (strcmp(arg, "-i") == 0) {
            svg_file = *argv++;
            argc--;
        } else if (strcmp(arg, "-o") == 0) {
            png_file = *argv++;
            argc--;
        } else if (strcmp(arg, "--dpi") == 0) {
            char *c = *argv++;
            dpi = (int)strtol(c, NULL, 0);
            argc--;
        }
    }
    if(!png_file) {
        const int len = strlen(svg_file);
        png_file = malloc(len+1);
        strncpy(png_file, svg_file, len-3);
        strcat(png_file, "png");
    }
    //printf("svg=%s, png=%s, dpi=%f\n", svg_file, png_file, dpi);
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
    if(w > MAX_WIDTH || h > MAX_WIDTH) {
		printf("Max image size exceeded.\n");
        goto error;
    }
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
	nsvgRasterize(rast, image, 0, 0, 1, img, w, h, w*4);
 	stbi_write_png(png_file, w, h, 4, img, w*4);
    return 0;
error:
	nsvgDeleteRasterizer(rast);
	nsvgDelete(image);
	return 1;
}
