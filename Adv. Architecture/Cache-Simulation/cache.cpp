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
int** allocateTag(unsigned memSize, unsigned lineSize, unsigned assoc, unsigned sets); // Tag array: associative memory
																					   // Write to memory
void memWrite(unsigned address, unsigned assoc,
	char** cache, int** tagArr, char* mainMem,
	const string & data, unsigned sets, unsigned lineSize);
// Read function
void memRead(unsigned address, unsigned assoc, char** cache, int** tagArr, char* mainMem, unsigned sets, unsigned lineSize);
// Translates a memory address into a tag, set, and offset
void bitTranslate(unsigned address, int & tag, unsigned & set, unsigned & offset, unsigned sets, unsigned lineSize);
// Checks if a set block is empty, returns the index of the first empty block it finds or -1 if it's empty.
int whichLine(unsigned address, int tag, int** tagArr, char** cache, unsigned set, unsigned lineSize, unsigned assoc, unsigned sets);
// Translates a cache address to a memory address.
unsigned cacheToMem(int** tagArr, unsigned lineSize, unsigned line, unsigned set, unsigned sets);
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
	int** tagArr = allocateTag(memSize, lineSize, assoc, sets); // Associative tag array

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
				memWrite(address, assoc, cache, tagArr, mainMem, writeData, sets, lineSize);
			}
			/*
			else if (inChar == 'F') {
			// Read access. Pass to the read function
			in >>
			memRead(address, assoc, cache, tagArr, mainMem, sets, lineSize);
			}
			*/
			break;
		case 'F':
			// Display the address, value in the simulated cache, and value in the simulated main memory.
			in >> address;
			memRead(address, assoc, cache, tagArr, mainMem, sets, lineSize);
			break;
		}
	}

	in.close();
	// Remember to free your memory, or the computer will explode.
	delete[] mainMem;
	delete[] cache;
	delete[] tagArr;
	system("pause");
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
	for (unsigned i = 0; i < size; i++) {
		ret[i] = 0;
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
			ret[i][j] = 0;
		}
	}
	cout << "Successfully allocated a cache of size " << size << " bytes." << endl;
	return ret;
}

int** allocateTag(unsigned memSize, unsigned lineSize, unsigned assoc, unsigned sets) {
	// Allocates an associative array to hold tags and translate addresses
	// We are assuming that a tag is a 4 byte integer.
	int** ret = nullptr;
	try {
		ret = new int*[sets]; // Our tag array will hold tags of each block in a 2d array of size (sets * associativity * tagBytes)
		for (unsigned i = 0; i < sets; i++) {
			ret[i] = new int[assoc];
		}
	}
	catch (bad_alloc &) {
		die("could not allocate memory.");
	}
	unsigned size = 0;
	for (unsigned i = 0; i < sets; i++) {
		for (unsigned j = 0; j < assoc; j++) {
			size += sizeof(int);
			ret[i][j] = -1;
		}
	}
	cout << "Successfully allocated a tag array of size " << size << " bytes." << endl;
	return ret;
}

void memWrite(unsigned address, unsigned assoc, char** cache, int** tagArr, char* mainMem, const string & data, unsigned sets, unsigned lineSize) {
	// Translate the main memory address to a cache address [set][offset]
	unsigned offset, set;
	int tag;
	bitTranslate(address, tag, set, offset, sets, lineSize);
	int block = whichLine(address, tag, tagArr, cache, set, lineSize, assoc, sets);
	if (block >= 0) {
		for (unsigned i = 0; i < data.length(); i++) {
			cache[set][block * lineSize + offset + i] = data.at(i); // Update cache.
			tagArr[set][block] = tag; // Update tag.
		}
	}
	else { // This means the cache set is full and the line we are writing to is not in memory. 
		   // We must evict the last line in the set back into main memory, shift each line forward, then write to the first line.
		   // This effectively turns the cache sets into queues (first in last out)
		   // Translate the line we are evicting to its memory address
		unsigned evictAddress = cacheToMem(tagArr, lineSize, assoc - 1, set, sets);
		// Copy line to memory and reset the tag in the tag memory
		for (unsigned i = 0; i < lineSize; i++) {
			mainMem[evictAddress + i] = cache[set][block + i];
		}
		tagArr[set][assoc - 1] = -1;
		// Shift every line forward by (lineSize) bytes
		for (unsigned i = 0; i < lineSize * assoc - lineSize; i++) {
			cache[set][i + lineSize] = cache[set][i];
		}
		// Write to the first line in the set
		for (unsigned i = 0; i < data.length(); i++) {
			cache[set][offset + i] = data.at(i); // Update cache.
			tagArr[set][0] = tag; // Update tag.
		}
	}
}

void memRead(unsigned address, unsigned assoc, char** cache, int** tagArr, char* mainMem, unsigned sets, unsigned lineSize) {
	// First check the cache for the memory block.
	unsigned set, offset;
	int tag, line = -1;
	bitTranslate(address, tag, set, offset, sets, lineSize);
	for (int i = 0; i < assoc; i++) {
		if (cacheToMem(tagArr, lineSize, i, set, sets) == (address - (address % lineSize))) {
			line = i;
		}
	}
	// Cache miss. Read from the given memory address then copy the memory block into cache.
	if (line == -1) {
		string writeData = "";
		for (unsigned i = 0; i < lineSize; i++) {
			writeData += mainMem[i + (address - (address % lineSize))];
		}
		memWrite(address, assoc, cache, tagArr, mainMem, writeData, sets, lineSize); // Write to memory
																					 // Get cache line containing the memory block
		for (int i = 0; i < assoc; i++) {
			if (cacheToMem(tagArr, lineSize, line, set, sets == (address - (address % lineSize)))) {
				line = i;
			}
		}
	}
	cout << hex << "Reading memory at address " << address << ". Value at address in main memory: " << (int)mainMem[address]
		<< " | Value at address in cache: " << (int)cache[set][line * lineSize + offset] << endl;
}


void bitTranslate(unsigned address, int & tag, unsigned & set, unsigned & offset, unsigned sets, unsigned lineSize) {
	// Translates memory index to give values for tag, set, and offset.
	tag = set = offset = 0;
	offset = (address & (lineSize - 1));
	address >>= bitCount(lineSize);
	set = (address & (sets - 1));
	address >>= bitCount(sets);
	tag = address;
}

int whichLine(unsigned address, int tag, int** tagArr, char** cache, unsigned set, unsigned lineSize, unsigned assoc, unsigned sets) {
	int ret; // Set index or -1 if no unused line is found
			 // First check if a given memory block is in the cache by comparing the tags. If it is, we want to write to that block.
			 // (May need to get rid of this function for turnin).
	for (ret = 0; ret < assoc; ret++) {
		if (cacheToMem(tagArr, lineSize, ret, set, sets) == (address - (address % lineSize))) {
			return ret;
		}
	}
	// Check if there is an empty block we can write to.
	int empty; // Keeps track of whether a line is empty. Is set to 0 as soon as we see a nonzero byte.
	for (ret = 0; ret < assoc; ret++) { // For each line in the set
		empty = 1;
		for (unsigned i = 0; i < lineSize; i++) {
			if (cache[set][ret * lineSize + i] != 0) {
				empty = 0;
				exit;
			}
			if (empty == 1 && i == (lineSize - 1))
				return ret; // Return the first empty line we see 
		}
	}
	return -1; // If we made it this far, none of our lines are empty.
}

unsigned cacheToMem(int** tagArr, unsigned lineSize, unsigned line, unsigned set, unsigned sets) {
	unsigned ret = 0;
	int tag = tagArr[set][line];
	ret |= tag;
	ret <<= (bitCount(sets));
	ret |= set;
	ret <<= (bitCount(lineSize));
	return ret;
}