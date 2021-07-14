#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
	NOTES:
		ADD ERROR CHECKING WHERE POSSIBLE IN EXISTING FUNCTIONS
		IMPLEMENT CONVERSION FROM FILE TO PROGRAM
		IMPLEMENT DRAWING FUNCTIONS
			-> header and px data as arguments
			-> if image exists already, use its header and px data as arguments
		IMPLEMENT SUPPORT FOR DIFFERENT COLOR DEPTHS (BITSPERPIXEL)
		IMPLEMENT CONVERSION FROM ONE BIT DEPTH TO ANOTHER
		IMPLEMENT COMPRESSION RELATED FUNCTIONS
	////
*/

#define BMP_DATA_OFFSET 54
#define BMP_INFO_HDR 40
#define BMP_PLANES 1

// Bitmap header structure
typedef struct {
	int filesize; // File size in bytes (4 bytes)
	int width; // Width of image (4 bytes)
	int height; // Height of image (4 bytes)
	short bitsperpixel; // Bits per pixel (2 bytes)
	int compression; // Compression (4 bytes)
	int imagesize;  // Image size (filesize - DATA_OFFSET) (4 bytes)
	int xperm; // X pixels per meter (4 bytes)
	int yperm; // Y pixels per meter (4 bytes)
	int colorsused; // Colors used (4 bytes)
	int importantcolors; // Important colors (4 bytes)
} bmp_hdr;

// Pixel structure for 16-bit and 24-bit color
typedef struct {
	char red;
	char green;
	char blue;
} bmp_px_rgb;

// int to 4-byte string, first character holds least significant bits
void itos(int input, char *output);
// short to 2-byte string, first character holds least significant bits
void shtos(short input, char *output);

// Dynamically creates new bmp_header with the inputted data values (used internally)
bmp_hdr* bmp_mk_hdr(int height, int width, short bitsperpixel);
// Converts bmp_header structure to valid bitmap header data in a string (used internally)
void bmp_hdr_buf(bmp_hdr *header, char *buffer);
// Converts array of bmp_px_rgb pixels to valid bitmap RGB pixel data (used internally)
void bmp_px_buf_rgb(bmp_px_rgb *pxdata, bmp_hdr *header, char *buffer);
// Creates new bitmap file to store given header and pixel data (deallocates both arguments given to it upon completion)
int bmp_mk_img(bmp_hdr *header, bmp_px_rgb *pxdata);

// Converts raw bitmap header data to bmp_header structure (used internally)
void bmp_buf_hdr(char *buffer, bmp_hdr *header);
// Converts raw bitmap pixel data to array of bmp_px_rgb pixels (used internally)
void bmp_buf_px_rgb(char *buffer, bmp_px_rgb *pxdata);
// Retrieves data from bitmap file and stores it in the proper structures
int bmp_get_img(char *filename);
