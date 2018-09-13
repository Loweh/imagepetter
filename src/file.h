#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PARAMETERS 7;
#define WIDTH_HEIGHT_SIZE 4;
#define OTHER_SIZE 1;

typedef struct image_t image_t;
struct image_t {
	FILE *file;
	char *filename;
	int filesize;
	int width;
	int height;
	int bitdepth;
	int colortype;
	int compmethod; //compression method
	int filtmethod; //filtering method
	int intemethod; //interlacing method
};

int find(char *string, int strlen, char *value, int valuelen); //i need to basically make a bad copy of strstr because i can't have something rely in null terms when dealing with raw image data

int image_init(image_t *img, char *filename);


