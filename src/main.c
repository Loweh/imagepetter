#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bitmap.h"

int main() {
	printf("Select an operation: \n"
		   "    Write (W)\n"
		   "    Read (R)\n");
		   
	char result = '\0';
	scanf("%c", &result);
	
	if (result == 'W') {
		char filename[260] = {0}; // 260 is apparently the maximum file path length in Windows
		int height = 0, width = 0, bitsperpixel = 0;
	
		printf("Please enter the following information: \n");
	
		printf("Filename: ");
		scanf("%s", filename);
	
		printf("Width: ");
		scanf("%u", &width);
	
		printf("Height: ");
		scanf("%u", &height);
	
		printf("Bit depth: ");
		scanf("%u", &bitsperpixel);
	
		bmp_hdr *header = bmp_mk_hdr(height, width, bitsperpixel);
		bmp_px_rgb *pxdata = malloc(sizeof(bmp_px_rgb) * height * width);
	
		time_t t;
		srand((unsigned) time(&t));
	
		for (int i = 0; i < height * width; i++) {
			if (bitsperpixel > 1) {
				pxdata[i].red = rand() % 255;
				pxdata[i].green = rand() % 255;
				pxdata[i].blue = rand() % 255;
			} else {
				pxdata[i].red = rand() % 2 ? 255 : 0;
				pxdata[i].green = pxdata[i].red;
				pxdata[i].blue = pxdata[i].red;
			}	
		}
		
		bmp_mk_img(filename, header, pxdata);
		
		free(header);
		free(pxdata);
	} else if (result == 'R') {
		char filename[260] = {0}; // 260 is apparently the maximum file path length in Windows
	
		printf("Filename: ");
		scanf("%s", filename);
	
		bmp_hdr *header = bmp_get_img_hdr(filename);
		bmp_px_rgb *pxdata = bmp_get_img_px(filename, header);
	
	
		printf("Header details:\n");
		printf("filesize: %i ", header->filesize);
		printf("width: %i ", header->width);
		printf("height: %i\n", header->height);
		printf("bit depth: %hi ", header->bitsperpixel);
		printf("compression: %i ", header->compression);
		printf("imagesize: %i\n", header->imagesize);
		printf("xperm: %i ", header->xperm);
		printf("yperm: %i ", header->yperm);	
		printf("colorsused: %i\n", header->colorsused);
		printf("importantcolors: %i\n", header->importantcolors);
	
		getchar();
	
		printf("Pixel details:\n");
	
		for (int i = 0; i < header->height; i++) {
			printf("scanline %i:\n", i+1);
			for (int k = 0; k < header->width; k++) {
				printf("px #%i, R: %i G: %i B: %i\n", k+1, pxdata[(i * header->width) + k].red, pxdata[(i * header->width) + k].green, pxdata[(i * header->width) + k].blue);
			}
		}
		
		free(header);
		free(pxdata);
	}
	
	printf("Press 'e' to exit.\n");
	while(getchar() != 'e');
	
	return 0;
}