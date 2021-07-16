#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
	NOTES:
		ADD ERROR CHECKING WHERE POSSIBLE IN EXISTING FUNCTIONS
		IMPLEMENT DRAWING FUNCTIONS
			-> header and px data as arguments
			-> if image exists already, use its header and px data as arguments
		IMPLEMENT SUPPORT FOR DIFFERENT COLOR DEPTHS (BITSPERPIXEL)
		IMPLEMENT CONVERSION FROM ONE BIT DEPTH TO ANOTHER
		IMPLEMENT COMPRESSION RELATED FUNCTIONS?
	////
*/

#define BMP_DATA_OFFSET 54
#define BMP_INFO_HDR 40
#define BMP_PLANES 1

// Bitmap header structure
typedef struct {
	unsigned int filesize; // File size in bytes (4 bytes)
	unsigned int width; // Width of image (4 bytes)
	unsigned int height; // Height of image (4 bytes)
	unsigned short bitsperpixel; // Bits per pixel (2 bytes)
	unsigned int compression; // Compression (4 bytes)
	unsigned int imagesize;  // Image size (filesize - DATA_OFFSET) (4 bytes)
	unsigned int xperm; // X pixels per meter (4 bytes)
	unsigned int yperm; // Y pixels per meter (4 bytes)
	unsigned int colorsused; // Colors used (4 bytes)
	unsigned int importantcolors; // Important colors (4 bytes)
} bmp_hdr;

// Pixel structure (all color bit depths are represented in RGB then converted to color palette data when needed)
// This conversion should also round to the nearest color in the palette if it doesn't fit exactly.
typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} bmp_px_rgb;

// int to 4-byte string, first character holds least significant bits
void itos(int input, char *output);
// short to 2-byte string, first character holds least significant bits
void shtos(short input, char *output);
// 4-byte string to int, first character holds least significant bits
int stoi(char *input);
// 2-byte string to short, first character holds least significant bits
short stosh(char *input);

// Dynamically creates new bmp_header with the inputted data values (used internally)
bmp_hdr* bmp_mk_hdr(int height, int width, short bitsperpixel);
// Converts bmp_header structure to valid bitmap header data in a string (used internally)
void bmp_hdr_buf(bmp_hdr *header, char *buffer);
// Converts array of bmp_px_rgb pixels to valid 24-bit bitmap RGB pixel data (used internally)
void bmp_px_buf_24bit(bmp_px_rgb *pxdata, bmp_hdr *header, char *buffer);
// Converts array of bmp_px_rgb pixels to valid 8-bit bitmap pixel data (used internally)
void bmp_px_buf_8bit(bmp_px_rgb *pxdata, bmp_hdr *header, char *buffer);
// Converts array of bmp_px_rgb pixels to valid 4-bit bitmap pixel data (used internally)
void bmp_px_buf_4bit(bmp_px_rgb *pxdata, bmp_hdr *header, char *buffer);
// Converts array of bmp_px_rgb pixels to valid 1-bit bitmap pixel data (used internally)
void bmp_px_buf_1bit(bmp_px_rgb *pxdata, bmp_hdr *header, char *buffer);
// Creates new bitmap file to store given header and pixel data (deallocates both arguments given to it upon completion)
int bmp_mk_img(char *filename, bmp_hdr *header, bmp_px_rgb *pxdata);

// Converts raw bitmap header data to bmp_header structure (used internally)
void bmp_buf_hdr(char *buffer, bmp_hdr *header);
// Converts raw 24-bit color bitmap pixel data to array of bmp_px_rgb pixels (used internally)
void bmp_buf_px_24bit(char *buffer, bmp_hdr *header, bmp_px_rgb *pxdata);
// Converts raw 8-bit color bitmap pixel data to array of bmp_px_rgb pixels (used internally)
void bmp_buf_px_8bit(char *buffer, bmp_hdr *header, bmp_px_rgb *pxdata);
// Converts raw 4-bit color bitmap pixel data to array of bmp_px_rgb pixels (used internally)
void bmp_buf_px_4bit(char *buffer, bmp_hdr *header, bmp_px_rgb *pxdata);
// Converts raw 1-bit color bitmap pixel data to array of bmp_px_rgb pixels (used internally)
void bmp_buf_px_1bit(char *buffer, bmp_hdr *header, bmp_px_rgb *pxdata);
// Retrieves header data from bitmap file and returns a pointer to the header structure holding that data
bmp_hdr* bmp_get_img_hdr(char *filename);
// Retrieves pixel data from bitmap file and returns the array of bmp_px_rgb pixels
bmp_px_rgb* bmp_get_img_px(char *filename, bmp_hdr *header);
