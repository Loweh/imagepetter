#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bitmap.h"

int main() {
	int height = 0, width = 0;
	
	printf("Please enter the following information: \n");
	
	printf("Height: ");
	scanf("%u", &height);
	
	printf("Width: ");
	scanf("%u", &width);
	
	bmp_hdr *header = bmp_mk_hdr(height, width, 24);
	bmp_px_rgb *pxdata = malloc(sizeof(bmp_px_rgb) * height * width);
	
	time_t t;
	srand((unsigned) time(&t));
	
	for (int i = 0; i < height * width; i++) {
		pxdata[i].red = rand() % 255;
		pxdata[i].green = rand() % 255;
		pxdata[i].blue = rand() % 255;
	}
	
	bmp_mk_img(header, pxdata);
	
	return 0;
}