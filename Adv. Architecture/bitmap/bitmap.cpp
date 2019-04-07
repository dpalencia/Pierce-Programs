#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "windows.h"
#define I_S 1024 // Image size
using namespace std;
void bmpRead(const string & fileName, char** bmp);
void die(const string & e); // Professor Hart's die function™
int convertToIndex(ifstream & i, float upper, float lower); // Converts a float to an appropriate array index in our bmp
int main() {
	BITMAPFILEHEADER bmfh; // Bitmap fileader
	BITMAPINFOHEADER bmih; // Bitmap info header
	char colorTable[1024]; // Greyscale color table
	for (int i = 0, j; i < 256; i++) {
		j = i * 4; // j offsets to get proper array indices for colorTable
		colorTable[j] = i;
		colorTable[j + 1] = i;
		colorTable[j + 2] = i;
		colorTable[j + 3] = i;
	}
	// DMA the array because we get a stack overflow if we don't
	char** bmp = nullptr; // Pointer to an array of pointers to arrays of chars (2d array)
	try {
		bmp = new char*[I_S]; // Create an array of pointers
		for (int i = 0; i < I_S; i++) {
			bmp[i] = new char[I_S];
		}
	}
	catch (bad_alloc &) {
		die("could not allocate memory.");
	}

	// Bitmap file header setup
	bmfh.bfType = 0x4d42;
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(bmfh) + sizeof(bmih) + sizeof(colorTable);
	bmfh.bfSize = bmfh.bfOffBits + I_S * I_S;

	// Bitmap info header setup
	bmih.biSize = 40;
	bmih.biWidth = I_S;
	bmih.biHeight = I_S;
	bmih.biPlanes = 1;
	bmih.biBitCount = 8; // 256 colors in each pixel
	bmih.biCompression = 0;
	bmih.biSizeImage = I_S * I_S;
	bmih.biXPelsPerMeter = 2835;  // Standard DPI
	bmih.biYPelsPerMeter = 2835;
	bmih.biClrUsed = 256;
	bmih.biClrImportant = 0;

	// Initialize our bitmap array to black
	for (int i = 0; i < I_S; i++) {
		for (int j = 0; j < I_S; j++) {
			bmp[i][j] = 0;
		}
	}

	bmpRead("L2Data10k.dat", bmp);

	// Time to write it all out
	ofstream o;
	o.open("cabpaths.bmp", ios::out + ios::binary);
	char* ptr;
	ptr = (char*)&bmfh;
	o.write(ptr, 14);
	ptr = (char*)&bmih;
	o.write(ptr, 40);
	ptr = &colorTable[0];
	o.write(ptr, sizeof(colorTable));
	for (int i = 0; i < I_S; i++) {
		for (int j = 0; j < I_S; j++) {
			o.write(&bmp[i][j], 1);
		}
	}
	o.close();
	system("mspaint cabpaths.bmp");

	delete[] bmp;
	return 0;
}


void bmpRead(const string & fileName, char** bmp) {
	ifstream i; // Input file
	i.open(fileName, ios::binary); // Open file in binary mode
	float latMax = 40.830519;
	float latMin = 40.700455;
	float longMax = 74.045033;
	float longMin = 73.914979;
	int x;
	int y;
	while (!i.eof()) {
		x = convertToIndex(i, latMax, latMin);
		y = 1023 - convertToIndex(i, longMax, longMin);
		//y = convertToIndexLat(i, longMax, longMin);
		if (0 <= x && x < 1024 && 0 <= y && y < 1024) {
			if (bmp[x][y] < 256)
				bmp[x][y]+= 50;
		}
	}
	i.close();
}

int convertToIndex(ifstream & i, float upper, float lower) {
	// Conversion from Latitude to array index:
	// ((point - lower) / (upper - lower)) * 1024 --> then round
	float ret;
	char bytes[4];
	int i_s = I_S; // to multiply register by 1024 we need it in a memory location
	i.read(bytes, 4);
	__asm {
		FLD bytes
		FABS
		FLD lower
		FSUBP st(1), st(0)
		FLD upper
		FLD lower
		FSUBP st(1), st(0)
		FDIVP st(1), st(0)
		FIMUL i_s
		FSTP ret
	}
	return round(ret); // Return the rounded value as an int
}

void die(const string & e) {
	cout << "ERROR: " << e << endl;
	system("pause");
	exit(EXIT_FAILURE);
}