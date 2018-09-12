#include "file.h"

int main(int argc, char **argv) {
	struct image img;

	img.filename = argv[1];

	int returnvalue = check_image(&img);

	printf("%i\n", returnvalue);

	return 0;
};
