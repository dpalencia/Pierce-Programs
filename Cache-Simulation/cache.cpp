#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath> // ceil() and log() for calculating bit count of an integer
using namespace std;
void die(const string & e);
unsigned bitCount(unsigned val); // Takes an unsigned value and returns how many bits are required to hold it.
unsigned setCount(unsigned cacheSize, unsigned lineSize, unsigned assoc); // Count # of sets
char* allocate(unsigned size); // Dynamically allocate main memory
char** allocateCache(unsigned cacheSize, unsigned lineSize, unsigned assoc, unsigned sets); // Dynamically allocate cache as as 2d array
char** allocateTag(unsigned memSize, unsigned lineSize, unsigned assoc, unsigned sets); // Tag array: associative memory
// Write to memory
void memWrite(unsigned address, unsigned assoc,
	char** cache, char** tagArr, char* mainMem, 
	const string & data, unsigned sets, unsigned lineSize); 
// Translates a memory address into a tag, set, and offset
void bitTranslate(unsigned address, unsigned & tag, unsigned & set, unsigned & offset, unsigned sets, unsigned lineSize); 
// Checks if a set block is empty, returns the index of the first empty block it finds or -1 if it's empty.
int setIsEmpty(char** cache, int set, unsigned lineSize, unsigned assoc);
int main() {
	ifstream in;
	in.open("input.txt");
	char inChar;
	unsigned lineSize; // Cache line size
	unsigned cacheSize; // Total cache size
	unsigned assoc; // Set associativity
	unsigned memSize; // Total main memory size
	unsigned address; // Memory address
	lineSize = cacheSize = assoc = memSize = 0;
	// The initial read. One instance of each of the first four items (A-D), so we read each of the four items once.
	for (unsigned i = 0; i < 4; i++) {
		string line;
		getline(in, line);
		istringstream lineIn(line);
		lineIn >> inChar;
		switch (inChar) {
		case 'A':
			lineIn >> lineSize;
			if (lineSize == 0 || lineSize % 2 != 0) {
				die("Invalud value " + to_string(lineSize) + " for Line Size.");
			}
			break;
		case 'B':
			lineIn >> cacheSize;
			if (cacheSize == 0 || cacheSize % 2 != 0) {
				die("Invalid value " + to_string(cacheSize) + " for Cache Size.");
			}
			break;
		case 'C':
			lineIn >> assoc;
			if (assoc == 0 || (assoc % 2 != 0)) {
				die("Invalid value " + to_string(assoc) + " for Associativity.");
			}
			break;
		case 'D':
			lineIn >> memSize;
			if (memSize == 0 || (memSize % 2 != 0)) {
				die("Invalid value " + to_string(memSize) + " for Memory Size.");
			}
			if (cacheSize > memSize * 0.10) {
				die("Cache size " + to_string(cacheSize) + "too large.");
			}
			break;
		}
	}
	// A couple more error checks.
	if (lineSize > cacheSize * 0.10) {
		die("Line Size " + to_string(lineSize) + " too large.");
	}
	// If one of these is still 0, the parameter was omitted
	if (!(lineSize && cacheSize && assoc && memSize)) {
		die("Missing parameter.");
	}
	// Now we intialize our memory system with some dynamic memory allocation
	// Get the number of sets.
	unsigned sets = setCount(cacheSize, lineSize, assoc); // # of sets
	char* mainMem = allocate(memSize); // Main memory
	char** cache = allocateCache(cacheSize, lineSize, assoc, sets); // Cache
	char** tagArr = allocateTag(memSize, lineSize, assoc, sets); // Associative tag array

	// The processing loop
	while (!in.eof()) {
		unsigned address; // Gets the memory address passed by input file
		string writeData; // Treated as a char array holding our data (in bytes)
		in >> inChar;
		switch (inChar) {
		case 'E':
			in >> address;
			in >> inChar;
			if (inChar == 'W') {
				// Write access. Pass to the write function
				in >> writeData;
				memWrite(address, assoc, tagArr, cache, mainMem, writeData, sets, lineSize);
			}
			else if (inChar == 'R') {
				// Read access. Pass to the read function
				in >> writeData;
			}
			break;
		case 'F':
			// Display the address, value in the simulated cache, and value in the simulated main memory.
			break;
		}
	}

	in.close(); 
	// Remember to free your memory, or the computer will explode.
	delete[] mainMem;
	delete[] cache;
	return 0;
}

void die(const string & e) {
	cout << e << endl;
	system("pause");
	exit(EXIT_FAILURE);
}

unsigned setCount(unsigned cacheSize, unsigned lineSize, unsigned assoc) {
	return cacheSize / lineSize / assoc;
}
unsigned bitCount(unsigned val) {
	return ceil(log(val) / log(2)); // Calculate # of bits required to hold a value
}

char* allocate(unsigned size) { // Allocate our block of simulated main memory.
	char* ret = nullptr;
	try {
		ret = new char[size];
	}
	catch (bad_alloc &) {
		die("could not allocate memory.");
	}
	cout << "Succesfully allocated a main memory of size " << sizeof(ret[0]) * size << " bytes." << endl;
	return ret;
}

char** allocateCache(unsigned cacheSize, unsigned lineSize, unsigned assoc, unsigned sets) {
	char** ret = nullptr;
	// Cache will be indexed like so:
	// [set #][line in set]
	try {
		ret = new char*[sets]; // Pointer to pointers to sets (arrays of bytes).
		for (unsigned i = 0; i < sets; i++) {
			ret[i] = new char[lineSize * assoc]; // lineSize * asoc = # of bytes per set. Each set has this # of bytes
		}
	}
	catch (bad_alloc &) {
		die("could not allocate memory.");
	}
	// Initialize the cache
	unsigned size = 0;
	for (unsigned i = 0; i < sets; i++) {
		for (unsigned j = 0; j < assoc * lineSize; j++) {
			size += 1;
			ret[i][j] = '0';
		}
	}
	cout << "Successfully allocated a cache of size " << size << " bytes." << endl;
	return ret;
}

char** allocateTag(unsigned memSize, unsigned lineSize, unsigned assoc, unsigned sets) { 
	// Allocates an associative array to hold tags and translate addresses
	// Get # of bits needed to hold a tag
	double tagBits = bitCount(memSize) - bitCount(sets) - bitCount(lineSize);
	unsigned tagBytes = ceil(tagBits / 8); // Just enough bytes to hold the # of bits per tag
	char** ret = nullptr;
	try {
		ret = new char*[sets]; // Our tag array will hold tags of each block in a 2d array of size (sets * associativity * tagBytes)
		for (unsigned i = 0; i < sets; i++) {
			ret[i] = new char[tagBytes * assoc];
		}
	}
	catch (bad_alloc &) {
		die("could not allocate memory.");
	}
	unsigned size = 0;
	for (unsigned i = 0; i < sets; i++) {
		for (unsigned j = 0; j < tagBytes * assoc; j++) {
			size += 1;
			ret[i][j] = '0';
		}
	}
	cout << "Successfully allocated a tag array of size " << size << " bytes." << endl;
	return ret;
}

void memWrite(unsigned address, unsigned assoc, char** cache, char** tagArr, char* mainMem, const string & data, unsigned sets, unsigned lineSize) {
	// Translate the main memory address to a cache address [set][offset]
	unsigned tag, offset, set;
	int block;
	bitTranslate(address, tag, set, offset, sets, lineSize);
	if (setIsEmpty(cache, set, lineSize, assoc)) {
		for (char c : data) {
			cache[set][]; 
		}
	}
}

void bitTranslate(unsigned address, unsigned & tag, unsigned & set, unsigned & offset, unsigned sets, unsigned lineSize) {
	// Translates memory index to give values for tag, set, and offset.
	tag = set = offset = 0;
	for (unsigned i = 0; i < bitCount(lineSize); i++) {
		offset |= (address & 1); // Write bit to offset
		address >>= 1; // Shift out last bit of address
	}
	for (unsigned i = 0; i < bitCount(sets); i++) {
		set |= (address & 1);
		address >>= 1;
	}
	tag = address;
}

int setIsEmpty(char** cache, unsigned set, unsigned lineSize, unsigned assoc) {
	for (unsigned i = 0; i < assoc; i++) { // For each line in the set
		for (unsigned j = 0; j < lineSize; j++) { // Check each byte in the line
			if (cache[set][i * lineSize + j] != '0') {
				return -1;
			}
		}
	}
	return 1; // If we couldn't find a taken line, the set is empty.
}