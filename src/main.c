#include "file.h"

int main(int argc, char **argv) {
	image_t img;

	int returnvalue = image_init(&img, argv[1]);

	printf("%i\n", returnvalue);

	return 0;
};
