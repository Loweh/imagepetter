#include "bitmap.h"

// Handy tools for working with bitmaps
void itos(int input, char *output) {
	// Since chars are only 1 byte, each byte of the int must have its own char
	for (int i = 0; i < sizeof(int); i++) {
		unsigned int temp = input;
		temp = temp << sizeof(int) * 8 - 8; // Clear other bytes to the left of the least significant byte
		temp = temp >> sizeof(int) * 8 - 8; // Return byte to the least significant position
		
		output[i] = 0; // Make sure output isn't contaminated with unrelated data before ORing its bits.
		output[i] = output[i] | temp;
		input = input >> 8;	// Set the next least significant byte as the least significant byte
	}
}

void shtos(short input, char *output) {
	// Since chars are only 1 byte, each byte of the short must have its own char
	for (int i = 0; i < sizeof(short); i++) {
		unsigned short temp = input;
		temp = temp << sizeof(short) * 8 - 8;
		temp = temp >> sizeof(short) * 8 - 8;
		
		output[i] = 0; // Make sure output isn't contaminated with unrelated data before ORing its bits.
		output[i] = output[i] | temp;
		input = input >> 8;	
	}
}

bmp_hdr* bmp_mk_hdr(int height, int width, short bitsperpixel) {
	bmp_hdr *header = malloc(sizeof(bmp_hdr));
	
	// imagesize has to be declared so early since filesize depends on it
	int imagesize = width * 3;
	if (imagesize % 4) { // Pad the size of the scanline so it becomes a multiple of 4 bytes
		imagesize += 4 - (imagesize % 4);
	}
	imagesize *= height; // Multiply size of scanlines by height to get the total size of the image (in bytes)
	
	int filesize = imagesize + BMP_DATA_OFFSET; // The total file size is just the image size plus the header size
	
	header->filesize = filesize;
	header->width = width;
	header->height = height;
	header->bitsperpixel = bitsperpixel;
	header->compression = 0;
	header->imagesize = imagesize;
	header->xperm = 0;
	header->yperm = 0;
	header->colorsused = 0;
	header->importantcolors = 0;
	
	return header;
}

void bmp_hdr_buf(bmp_hdr *header, char *buffer) {
	// Clear buffer for safety
	for (int i = 0; i < BMP_DATA_OFFSET; i++) {
		buffer[i] = 0;
	}
	
	// Every BMP file starts with a two byte signature "BM"
	strncpy(buffer, "BM", 2);
	int offset = 2;

	// Filesize (4 bytes)
	itos(header->filesize, buffer + offset); 
	offset += 4;
	
	// Restricted (4 bytes)
	itos(0, buffer + offset);
	offset += 4;
	
	// Data offset (4 bytes)
	itos(BMP_DATA_OFFSET, buffer + offset);
	offset += 4;
	
	// Info header (4 bytes)
	itos(BMP_INFO_HDR, buffer + offset);
	offset += 4;
	
	// Width (4 bytes)
	itos(header->width, buffer + offset);
	offset += 4;
	
	// Height (4 bytes)
	itos(header->height, buffer + offset);
	offset += 4;
	
	// Planes (2 bytes so it gets special treatment)
	shtos(BMP_PLANES, buffer + offset);
	offset += 2;
	
	// Bits per pixel (also 2 bytes)
	shtos(header->bitsperpixel, buffer + offset);
	offset += 2;
	
	//Compression (4 bytes)
	itos(header->compression, buffer + offset);
	offset += 4;
	
	// Image size (4 bytes)
	itos(header->imagesize, buffer + offset);
	offset += 4;
	
	// X pixels per meter (4 bytes)
	itos(header->xperm, buffer + offset);
	offset += 4;
	
	// Y pixels per meter (4 bytes)
	itos(header->yperm, buffer + offset);
	offset += 4;
	
	// Colors used (4 bytes)
	itos(header->colorsused, buffer + offset);
	offset += 4;
	
	// Important colors (4 bytes)
	itos(header->importantcolors, buffer + offset);
	offset += 4;
}

void bmp_px_buf_rgb(bmp_px_rgb *pxdata, bmp_hdr *header, char *buffer) {
	// Clear buffer for safety
	for (int i = 0; i < header->imagesize; i++) {
		buffer[i] = 0;
	}
	
	// A scanline's length in bytes
	int sclbytes = header->imagesize / header->height;
	
	// Iterate through the number of scanlines
	for (int i = 0; i < header->height; i++) {
		// Iterate through the bytes of each scanline
		for (int k = 0; k < sclbytes; k++) {
			// If the byte is not padding (the current byte is not greater than the number of pixels wide * 3 for each rgb value)
			if (k < header->width * 3) {
				if (!((k+1) % 3)) { // If the byte is the third byte for that pixel, it is red
					buffer[(i * sclbytes) + k] = pxdata[(i * header->width) + k/3].red;
				} else if (!((k+2) % 3)) { // If the byte is the second byte for that pixel, it is green
					buffer[(i * sclbytes) + k] = pxdata[(i * header->width) + k/3].green;
				} else { // If the byte is the first byte for that pixel, it is blue
					buffer[(i * sclbytes) + k] = pxdata[(i * header->width) + k/3].blue;
				}
			} // Since the buffer is set to all 0s, assigning 0 as a value to the padding bytes is not necessary.
		}
	}
}

int bmp_mk_img(bmp_hdr *header, bmp_px_rgb *pxdata) {
	// Convert header data to string
	char headerdata[BMP_DATA_OFFSET] = {0}; // Since the header is a fixed size, dynamic memory allocations is not necessary.
	bmp_hdr_buf(header, headerdata);
	// Convert pixel data to string
	char *pixeldata = malloc(sizeof(char) * header->imagesize);
	bmp_px_buf_rgb(pxdata, header, pixeldata);
	
	// Write to file
	FILE *bmp;
	bmp = fopen("test.bmp", "wb+");	
	
	int retvalue = fwrite(headerdata, sizeof(char), sizeof(headerdata), bmp);
	retvalue = fwrite(pixeldata, sizeof(char), sizeof(char) * header->imagesize, bmp);
	
	free(pixeldata);
	free(header);
	fclose(bmp);
	return 0;
}

void bmp_buf_hdr(char *buffer, bmp_hdr *header) {
	// To do
}

void bmp_buf_px_rgb(char *buffer, bmp_px_rgb *pxdata) {
	// To do
}

int bmp_get_img(char *filename) {
	// To do
}