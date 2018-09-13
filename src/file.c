#include "file.h"

int find(char *string, int strlen, char *value, int valuelen) {
    int i;

    for (i = 0; i < strlen; i++) {
        if (!memcmp(string + i, value, valuelen)) {
            return i;
        };
    };

    return -1;
};

int image_init(image_t *img, char *filename) {
	img->filename = filename;

    if (strstr(img->filename, ".png") == NULL) {
		return 1;
	};

	img->file = fopen(img->filename, "r");

	if (img->file == NULL) {
		return 2;
	};

	fseek(img->file, 0, SEEK_END);
	img->filesize = ftell(img->file);
	rewind(img->file);

	char *data = (char *) malloc(img->filesize);
    fread(data, img->filesize, 1, img->file);
    fclose(img->file);

    int chunk = find(data, img->filesize, "IHDR", 4);

    if(chunk == 0) {
        return 3;
    };

	int i, j;

	img->width = 0;
	img->height = 0;

	for (i = 0; i < 8; i++) {
		int byte = (unsigned char) data[chunk + 4 + i];

		if (i < 4) {
			byte = byte << (3 - i) * 8;

			img->width = img->width | byte;
		} else {
			j = i - 4;

			byte = byte << (3 - j) * 8;

			img->height = img->height | byte;
		};
	};

    img->bitdepth = (int) data[chunk + 12];
    img->colortype = (int) data[chunk + 13];
    img->compmethod = (int) data[chunk + 14];
    img->filtmethod = (int) data[chunk + 15];
    img->intemethod = (int) data[chunk + 16];

    free(data);

    return 0;
};


