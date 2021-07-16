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

int stoi(char *input) {
	// Since local ints can be returned rather easily, unlike local pointers, there is no need for an output argument.
	int output = 0;
	
	for (int i = 0; i < sizeof(int); i++) {
		unsigned int temp = input[i];
		temp = temp << i * 8; // Since the least significant bits are in the first character, each character after the first must be left shifed to the proper position
		output = output | temp;
	}
	
	return output;
}

short stosh(char *input) {
	// Local shorts are also easy to return so no output argument needed here either.
	short output = 0;
	
	for (int i = 0; i < sizeof(short); i++) {
		unsigned short temp = input[i];
		temp = temp << i * 8; // Since the least significant bits are in the first character, each character after the first must be left shifed to the proper position
		output = output | temp;
	}
	
	return output;
}

bmp_hdr* bmp_mk_hdr(int height, int width, short bitsperpixel) {
	bmp_hdr *header = malloc(sizeof(bmp_hdr));
	
	// imagesize has to be declared so early since filesize depends on it
	int imagesize = 0, filesize = 0;
	switch (bitsperpixel) {
		case 1:
			if (width % 8) { // Find how many bytes are needed to fit width amount of bits
				imagesize = width / 8 + 1;
			} else {
				imagesize = width / 8;
			}
			
			if (imagesize % 4) { // Pad the size of the scanline so it becomes a multiple of 4 bytes
				imagesize += 4 - (imagesize % 4);
			}
			
			imagesize *= height;
			filesize = imagesize + BMP_DATA_OFFSET + 8; // The total file size is just the image size plus the header size (8 is the palette size)
			break;
		case 4:
			break;
		case 8:
			break;
		case 24:
			imagesize = width * 3;
			
			if (imagesize % 4) { // Pad the size of the scanline so it becomes a multiple of 4 bytes
				imagesize += 4 - (imagesize % 4);
			}
			
			imagesize *= height; // Multiply size of scanlines by height to get the total size of the image (in bytes)
			filesize = imagesize + BMP_DATA_OFFSET; // The total file size is just the image size plus the header size
			break;
		default:
			break;
	}
	
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

void bmp_px_buf_24bit(bmp_px_rgb *pxdata, bmp_hdr *header, char *buffer) {
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
				if (!((k + 1) % 3)) { // If the byte is the third byte for that pixel, it is red
					buffer[(i * sclbytes) + k] = pxdata[(i * header->width) + k / 3].red;
				} else if (!((k + 2) % 3)) { // If the byte is the second byte for that pixel, it is green
					buffer[(i * sclbytes) + k] = pxdata[(i * header->width) + k / 3].green;
				} else { // If the byte is the first byte for that pixel, it is blue
					buffer[(i * sclbytes) + k] = pxdata[(i * header->width) + k / 3].blue;
				}
			} // Since the buffer is set to all 0s, assigning 0 as a value to the padding bytes is not necessary.
		}
	}
}

void bmp_px_buf_8bit(bmp_px_rgb *pxdata, bmp_hdr *header, char *buffer) {
	// To do (FIND A GOOD WAY TO STORE/GENERATE THE ENTIRE PALETTE)
}

void bmp_px_buf_4bit(bmp_px_rgb *pxdata, bmp_hdr *header, char *buffer) {
	// To do
}
void bmp_px_buf_1bit(bmp_px_rgb *pxdata, bmp_hdr *header, char *buffer) {
	// Clear buffer for safety
	for (int i = 0; i < header->imagesize; i++) {
		buffer[i] = 0;
	}

	// Still uses a palette following the typical 4 bytes, 1 for red, green, blue and padding, so that must be added.
	int paletteoffset = 8;
	char palette[8] = {0, 0, 0, 0, 255, 255, 255, 0};
	memcpy(buffer, palette, paletteoffset);
	
	// A scanline's length in bytes
	int sclbytes = header->imagesize / header->height;
	
	for (int i = 0; i < header->height; i++) {
		//Iterate through the bits of each scanline
		for (int k = 0; k < sclbytes * 8; k++) {
			if (k < header->width) {
				unsigned int temp = pxdata[(i * header->width) + k].red ? 1 : 0; // Since the only 2 colors are 0,0,0 and 255,255,255, we don't need to read each rgb value.
				temp = temp << 7 - k % 8; // The first pixel should be the most significant bit
				
				buffer[paletteoffset + (i * sclbytes) + k / 8] = buffer[paletteoffset + (i * sclbytes) + k / 8] | temp;
			}
		}
	} 
}

int bmp_mk_img(char *filename, bmp_hdr *header, bmp_px_rgb *pxdata) {
	// Convert header data to string
	char hdrbuffer[BMP_DATA_OFFSET] = {0}; // Since the header is a fixed size, dynamic memory allocations is not necessary.
	bmp_hdr_buf(header, hdrbuffer);
	
	// Get size of color table (if applicable)
	int colortable = 0;
	
	if (header->bitsperpixel < 16) {
		if (header->colorsused == 0) {
			colortable = 1 << header->bitsperpixel; // I didn't want to include the math library just for this line of code, so I get 2^bitsperpixel by using left shifts.
		} else {
			colortable = header->colorsused;
		}
	}
	
	colortable *= 4; // Each color table entry has 4 bytes, blue, green, red, and a padding 0 byte
	
	// Convert pixel data to string
	char *pxbuffer = malloc(sizeof(char) * (header->imagesize + colortable));
	
	switch (header->bitsperpixel) {
		case 1:
			bmp_px_buf_1bit(pxdata, header, pxbuffer);
			break;
		case 4:
			bmp_px_buf_4bit(pxdata, header, pxbuffer);
			break;
		case 8:
			bmp_px_buf_8bit(pxdata, header, pxbuffer);
			break;
		case 24:
			bmp_px_buf_24bit(pxdata, header, pxbuffer);
			break;
		default:
			break;
	}
	
	// Write to file
	FILE *bmp;
	bmp = fopen(filename, "wb+");	
	
	int retvalue = fwrite(hdrbuffer, sizeof(char), sizeof(hdrbuffer), bmp);
	retvalue = fwrite(pxbuffer, sizeof(char), header->imagesize + colortable, bmp);
	
	free(pxbuffer);
	free(header);
	fclose(bmp);
	return 0;
}



void bmp_buf_hdr(char *buffer, bmp_hdr *header) {
	int offset = 2; // The signature (2 bytes long) is not needed, so the offset can start after it
	
	// File size (4 bytes)
	header->filesize = stoi(buffer + offset);
	offset += 16; // The reserved, dataoffset, and infoheader size (all 4 bytes) not needed, so are added to the offset
	
	// Width (4 bytes)
	header->width = stoi(buffer + offset);
	offset += 4;
	
	// Height (4 bytes)
	header->height = stoi(buffer + offset);
	offset += 6; // The planes attribute (2 bytes) is not needed, so is added to the offset
	
	// Bits per pixel (2 bytes)
	header->bitsperpixel = stosh(buffer + offset);
	offset += 2;
	
	// Compression (4 bytes)
	header->compression = stoi(buffer + offset);
	offset += 4; 
	
	// Image size (4 bytes)
	header->imagesize = stoi(buffer + offset);
	offset += 4; 
	
	// X pixels per meter (4 bytes)
	header->xperm = stoi(buffer + offset);
	offset += 4; 
	
	// Y pixels per meter (4 bytes)
	header->yperm = stoi(buffer + offset);
	offset += 4; 
	
	// Colors used (4 bytes)
	header->colorsused = stoi(buffer + offset);
	offset += 4; 
	
	// Important colors (4 bytes)
	header->importantcolors = stoi(buffer + offset);
	offset += 4; 	
}

void bmp_buf_px_24bit(char *buffer, bmp_hdr *header, bmp_px_rgb *pxdata) {	
	// A scanline's length in bytes
	int sclbytes = header->imagesize / header->height;
	
	for (int i = 0; i < header->height; i++) {
		for (int k = 0; k < sclbytes; k++) {
			if (k < header->width * 3) {
				if (!((k+1) % 3)) { // If the byte is the third byte for that pixel, it is red
					pxdata[(i * header->width) + k/3].red = buffer[(i * sclbytes) + k];
				} else if (!((k+2) % 3)) { // If the byte is the second byte for that pixel, it is green
					pxdata[(i * header->width) + k/3].green = buffer[(i * sclbytes) + k];
				} else { // If the byte is the first byte for that pixel, it is blue
					pxdata[(i * header->width) + k/3].blue = buffer[(i * sclbytes) + k];
				}
			}
		}
	}
}

void bmp_buf_px_8bit(char *buffer, bmp_hdr *header, bmp_px_rgb *pxdata) {
	// If colorsused = 0, the color table has all 256 colors
	int colors = header->colorsused ? header->colorsused : 256;
	// Each color table entry has 4 bytes, red, green, blue and a 0 byte, so the image data offset is the number of colors used * 4.
	int paletteoffset = colors * 4;
	// A scanline's length in bytes
	int sclbytes = header->imagesize / header->height;
	
	for (int i = 0; i < header->height; i++) {
		for (int k = 0; k < sclbytes; k++) {
			if (k < header->width) { // If the byte is a pixel and not padding
				unsigned char pixelcolor = buffer[paletteoffset + (i * sclbytes) + k];
				pxdata[(i * header->width) + k].red = buffer[pixelcolor * 4 + 2];
				pxdata[(i * header->width) + k].green = buffer[pixelcolor * 4 + 1];
				pxdata[(i * header->width) + k].blue = buffer[pixelcolor * 4];
			}
		}
	}
}

void bmp_buf_px_4bit(char *buffer, bmp_hdr *header, bmp_px_rgb *pxdata) {
	// If colorsused = 0, the color table has all 16 colors
	int colors = header->colorsused ? header->colorsused : 16;
	// Each color table entry has 4 bytes, red, green, blue and a 0 byte, so the image data offset is the number of colors used * 4.
	int paletteoffset = colors * 4;
	// A scanline's length in bytes
	int sclbytes = header->imagesize / header->height;
	
	// Since 4-bit color means each pixel is represented by half a byte some bit manipulation is needed here
	for (int i = 0; i < header->height; i++) {
		for (int k = 0; k < sclbytes * 2; k++) { // Each pixel is half a byte so this must be doubled to iterate through each pixel + padding.
			if (k < header->width) { // If the byte is a pixel and not padding
				unsigned char pixelcolor = buffer[paletteoffset + (i * sclbytes) + k / 2]; // Stay on the same byte until both 4 bit chunks have been read
				
				if (k % 2) { // If k is odd, then the corresponding 4 bits will be the least significant bits
					pixelcolor = pixelcolor << 4; // Since we want the LSBs, we need to push out the MSBs with a left shift
					pixelcolor = pixelcolor >> 4; // Bring it back so the value is correct
				} else { // If k is even then the corresponding 4 bits will be the most significant bits
					pixelcolor = pixelcolor >> 4; // Push out the LSBs and also brings the bits down so the value shows properly
				}
				
				pxdata[(i * header->width) + k].red = buffer[pixelcolor * 4 + 2];
				pxdata[(i * header->width) + k].green = buffer[pixelcolor * 4 + 1];
				pxdata[(i * header->width) + k].blue = buffer[pixelcolor * 4];
			}
		}
	}
}
void bmp_buf_px_1bit(char *buffer, bmp_hdr *header, bmp_px_rgb *pxdata) {
	// Even 1-bit color still uses a color table, the first entry is black and the second is white.
	int colors = 2;
	// Each color table entry has 4 bytes, red, green, blue and a 0 byte, so the image data offset is the number of colors used * 4.
	int paletteoffset = colors * 4;
	// A scanline's length in bytes
	int sclbytes = header->imagesize / header->height;
	
	// Since 1-bit color means each pixel is represented by 1 bit, some bit manipulation is needed here
	for (int i = 0; i < header->height; i++) {
		for (int k = 0; k < sclbytes * 8; k++) { // Each pixel is 1 bit, so this must be multiplied by 8 to iterate through each pixel + padding.
			if (k < header->width) { // If the byte is a pixel and not padding
				unsigned char pixelcolor = buffer[paletteoffset + (i * sclbytes) + k / 8]; // Stay on the same byte until all 8 bits are read
				
				pixelcolor = pixelcolor << k % 8; // Shift left to clear any previously read bits
				pixelcolor = pixelcolor >> 7; // Shift right to set the desired bit as LSB
				
				pxdata[(i * header->width) + k].red = pixelcolor ? 255 : 0; // When the bit is 0, it corresponds to black, when it is 1, it is white
				pxdata[(i * header->width) + k].green = pixelcolor ? 255 : 0;
				pxdata[(i * header->width) + k].blue = pixelcolor ? 255 : 0;;
			}
		}
	}
}

bmp_hdr* bmp_get_img_hdr(char *filename) {
	FILE *bmp;
	bmp = fopen(filename, "rb+");	
	
	bmp_hdr *header = malloc(sizeof(bmp_hdr));
	char hdrbuffer[BMP_DATA_OFFSET] = {0};
	
	fread(hdrbuffer, sizeof(char), BMP_DATA_OFFSET, bmp);
	
	bmp_buf_hdr(hdrbuffer, header);
	
	// Since the user will want to use the header after this function is called, it cannot be
	// freed here, so it is important to remember to free it once it's done being used.
	fclose(bmp);
	return header;
}

bmp_px_rgb* bmp_get_img_px(char *filename, bmp_hdr *header) {
	FILE *bmp;
	bmp = fopen(filename, "rb+");	
	fseek(bmp, BMP_DATA_OFFSET, SEEK_SET); // Offset the read to get past the header
	
	bmp_px_rgb *pxdata = malloc(sizeof(bmp_px_rgb) * header->height * header->width);
	char *pxbuffer;
	int colortable = 0;
	
	if (header->bitsperpixel < 16) {
		if (header->colorsused == 0) {
			colortable = 1 << header->bitsperpixel; // I didn't want to include the math library just for this line of code, so I get 2^bitsperpixel by using left shifts.
		} else {
			colortable = header->colorsused;
		}
	}
	
	colortable *= 4; // Each color table entry has 4 bytes, blue, green, red, and a padding 0 byte
	
	pxbuffer = malloc(sizeof(char) * (header->imagesize + colortable)); 
	
	fread(pxbuffer, sizeof(char), header->imagesize + colortable, bmp);

	switch (header->bitsperpixel) {
		case 1:
			bmp_buf_px_1bit(pxbuffer, header, pxdata);
			break;
		case 4:
			bmp_buf_px_4bit(pxbuffer, header, pxdata);
			break;
		case 8:
			bmp_buf_px_8bit(pxbuffer, header, pxdata);
			break;
		case 24:
			bmp_buf_px_24bit(pxbuffer, header, pxdata);
			break;
		default:
			break;
	}	
	
	// Since the user will want to use the pxdata after this function is called, it cannot be
	// freed here, so it is important to remember to free it once it's done being used.
	free(pxbuffer);
	fclose(bmp);
	return pxdata;
}