#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define PARAMETERS 7;
#define WIDTH_HEIGHT_SIZE 4;
#define OTHER_SIZE 1;

struct image {
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

int find(char *string, int strlen, char *value) {  //i need to basically make a bad copy of strstr because i can't have something rely in null terms when dealing with raw image data
	int valuelen = 0;

	while (1) {
		if (value[valuelen] == '\0') {
			break;
		};

		valuelen++;
	};

	int i;
	char *segment = (char *) malloc(valuelen);

	for (i = 0; i < strlen; i++) {
		strncpy(segment, string + i, valuelen);

		if (!strcmp(segment, value)) {
			free(segment);
			return i;
		};
	};

	free(segment);
	return -1;
};

int check_image(struct image *img) {
	struct stat st;

	if (stat(img->filename, &st) != 0) {
		return 1;
	} else if (strstr(img->filename, ".png") == NULL) {
		return 2;
	};

	img->filesize = (int) st.st_size;

	char *data = (char *) malloc(img->filesize);

    img->file = fopen(img->filename, "r"); 
    fread(data, img->filesize, 1, img->file);
    fclose(img->file);

	int chunk = find(data, img->filesize, "IHDR");

	if(chunk == 0) {
		return 3;
	};

	img->bitdepth = (int) data[chunk + 12];
	img->colortype = (int) data[chunk + 13];
	img->compmethod = (int) data[chunk + 14];
	img->filtmethod = (int) data[chunk + 15];
	img->intemethod = (int) data[chunk + 16];

    free(data);

	return 0;
};



