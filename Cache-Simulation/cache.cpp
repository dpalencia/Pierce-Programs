#include <iostream>
#include <fstream>
#include <string>
using namespace std;
char* allocate(unsigned size); // Dynamically allocate main memory
char** allocateCache(unsigned cacheSize, unsigned lineSize, unsigned assoc); // Dynamically allocate cache as as 2d array
void die(const string & e);
int main() {
	ifstream in;
	in.open("input.txt");
	unsigned inChar; // Work character we will read into
	unsigned lineSize; // Cache line size
	unsigned cacheSize; // Total cache size
	unsigned assoc; // Set associativity
	unsigned memSize; // Total main memory size
	unsigned address; // Memory address
	lineSize = cacheSize = assoc = memSize = 0;
	// The initial read. One instance of each of the first four items (A-D), so we read each of the four items once.
	for (unsigned i = 0; i < 4; i++) {
		in >> inChar;
		switch (inChar) {
		case 'A':
			in >> lineSize;
			if (lineSize == 0 || lineSize % 2 == 0) {
				cout << "Invalid value " << lineSize << "for Line Size.";
				exit(EXIT_FAILURE);
			}
			break;
		case 'B':
			in >> cacheSize;
			if (cacheSize == 0 || cacheSize % 2 == 0) {
				cout << "Invalid value " << cacheSize << "for Line Size.";
				exit(EXIT_FAILURE);
			}
			if (lineSize > cacheSize * 0.10) {
				cout << "Line Size " << lineSize << " too large." << endl;
				exit(EXIT_FAILURE);
			}
			break;
		case 'C':
			in >> assoc;
			if (assoc == 0 || (assoc % 2 == 0)) {
				cout << "Invalid value " << assoc << "for Line Size.";
				exit(EXIT_FAILURE);
			}
			break;
		case 'D':
			in >> memSize;
			if (memSize == 0 || (memSize % 2 == 0)) {
				cout << "Invalid value " << memSize << "for Line Size.";
			}
			if (cacheSize > memSize * 0.10) {
				cout << "Cache Size " << cacheSize << " too large." << endl;
				exit(EXIT_FAILURE);
			}
			break;
		}
	}
	// If one of these is still 0, the parameter was omitted
	if (!(lineSize && cacheSize && assoc && memSize)) {
		cout << "Missing parameter." << endl;
		exit(EXIT_FAILURE);
	}
	// Now we intialize our memory system; dynamic memory allocation
	char* mainMem = allocate(memSize);
	char** cache = allocateCache(cacheSize, lineSize, assoc);

	// Create the associative tag array here

	// The processing loop
	while (!in.eof()) {
		in >> inChar;
		switch (inChar) {
		case 'E':
			in >> address;
			in >> inChar;
			if (inChar == 'W') {
				// Write access. Pass to the write function
			}
			else if (inChar == 'R') {
				// Read access. Pass to the read function
			}
			break;
		case 'F':
			// Display the address, value in the simulated cache, and value in the simulated main memory.
			break;
		}
	}

	in.close(); 
	// Remember to free your memory, or the computer will explode.
	return 0;
}
void die(const string & e) {
	cout << "Error: " << e << endl;
	exit(EXIT_FAILURE);
}
char* allocate(unsigned size) { // Allocate a block of memory.
	char* ret = nullptr;
	try {
		ret = new char[size];
	}
	catch (bad_alloc &) {
		die("could not allocate memory.");
	}
	return ret;
}

char** allocateCache(unsigned cacheSize, unsigned lineSize, unsigned assoc) {
	char** ret = nullptr;
	unsigned sets = cacheSize / lineSize / assoc;
	// Cache will be indexed like so:
	// [set #][line in set]
	try {
		char** ret = new char*[sets]; // Pointer to pointers to sets (arrays of bytes).
		for (unsigned i = 0; i < sets; i++) {
			ret[i] = new char[lineSize * assoc]; // lineSize * asoc = # of bytes per set. Each set has this # of bytes
		}
	}
	catch (bad_alloc &) {
		die("could not allocate memory.");
	}
	return ret;
}