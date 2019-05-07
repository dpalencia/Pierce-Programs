// Palencia, Daniel		CS 546 Section 13495 5-6-19
// Third Laboratory Assignment - Cache Simulation
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath> // log
#include <iomanip> // setw
using namespace std;
void die(const string & e);
unsigned bitCount(unsigned val);

class AddressConverter { 
// This class does these conversions depending on input:
	// (lineSize, number of sets, address) -> Tag, Set, Offset
	// (Tag, Set, Offset) -> Address
// Or it takes a a tag, set, and offset and gives back a memory address.
public:
	unsigned tag, set, offset, address;
	AddressConverter(unsigned lineSize, unsigned setCount, unsigned inAddress) {
		tag = set = offset = 0;
		address = inAddress;
		offset = (inAddress & (lineSize - 1));
		inAddress >>= bitCount(lineSize);
		set = (inAddress & (setCount - 1));
		inAddress >>= bitCount(setCount);
		tag = inAddress;
	}
	AddressConverter(unsigned lineSize, unsigned setCount, unsigned inTag, unsigned inSet, unsigned offset) {
		address = 0;
		address |= inTag;
		address <<= (bitCount(setCount));
		address |= inSet;
		address <<= (bitCount(lineSize));
		address |= offset;
	}
};

class CacheSys { // Includes dynamically allocated cache memory and associative memory for tags
public:
	int* mainMem; // Indexed by:  Line, Offset
	int*** cacheMem; // Indexed by: set, line, offset
	int** tagMem; // Indexed by: set, line
	unsigned lineSize;
	unsigned memSize;
	unsigned assoc;
	unsigned sets;
	unsigned cacheSize;
	unsigned offset;
	CacheSys(unsigned inMemSize, unsigned inAssoc, unsigned inLineSize, unsigned inCacheSize); // Creates cache object and intializes cache memory.
	~CacheSys(); // Deallocates memory.
	void write(unsigned address, int data);
	void read(unsigned address);
	int tagCheck(unsigned tag, unsigned set); // Checks tag against associative tag memory
};

CacheSys::CacheSys(unsigned inMemSize, unsigned inAssoc, unsigned inLineSize, unsigned inCacheSize) {
	// Allocate and initialize memory.
	memSize = inMemSize;
	unsigned memIndex = memSize / 4;
	assoc = inAssoc;
	lineSize = inLineSize;
	cacheSize = inCacheSize;
	sets = cacheSize / lineSize / assoc;
	mainMem = nullptr;
	cacheMem = nullptr;
	tagMem = nullptr;
	try {
		mainMem = new int[memIndex];
		cacheMem = new int**[sets];
		for (unsigned i = 0; i < sets; i++) {
			cacheMem[i] = new int*[assoc];
			for (unsigned j = 0; j < assoc; j++) {
				cacheMem[i][j] = new int[lineSize / 4];
			}
		}
		tagMem = new int*[sets];
		for (unsigned i = 0; i < sets; i++) {
			tagMem[i] = new int[assoc];
		}
	}
	catch (bad_alloc &) {
		die("could not allocate memory.");
	}
	// Initialize everything to -1.
	for (unsigned i = 0; i < memIndex; i++) {
		mainMem[i] = -1;
	}
	for (unsigned i = 0; i < sets; i++) {
		for (unsigned j = 0; j < assoc; j++) {
			tagMem[i][j] = -1;
			for (unsigned k = 0; k < lineSize / 4; k++) {
				cacheMem[i][j][k] = -1;
			}
		}
	}
}

int CacheSys::tagCheck(unsigned tag, unsigned set) {
	// Check input tag against stored tags and return the line containing it, or return -1 if the tag is not found.
	for (unsigned i = 0; i < assoc; i++) {
		if (tagMem[set][i] == tag) 
				return i;
	}
	return -1;
}

void CacheSys::write(unsigned address, int data) {
	AddressConverter addrVals(lineSize, sets, address);
	int line = tagCheck(addrVals.tag, addrVals.set);
	if (line == -1) {
		// Check if the set is full
		for (unsigned i = 0; i < assoc; i++) {
			if (tagMem[addrVals.set][i] == -1) {
				// Empty, writeable line found. Write to it.
				cacheMem[addrVals.set][i][addrVals.offset / 4] = data;
				tagMem[addrVals.set][i] = addrVals.tag;
				return;
			}
		}
			// Copy the first element in the set back to main memory.
			AddressConverter cacheVals(lineSize, sets, tagMem[addrVals.set][0], addrVals.set, 0);
			for (unsigned i = 0; i < lineSize / 4; i++) {
				mainMem[(cacheVals.address / 4) + i] = cacheMem[addrVals.set][0][i];
			}
			// Shift every line in the cache back by one
			for (unsigned i = 0; i < assoc - 1; i++) {
				for (unsigned j = 0; j < (lineSize / 4) - 1; j++) {
					cacheMem[addrVals.set][i][j] = cacheMem[addrVals.set][i + 1][j];
				}
				tagMem[addrVals.set][i] = tagMem[addrVals.set][i + 1];
			}
			// Write to the last line in the cache.
			cacheMem[addrVals.set][assoc - 1][addrVals.offset / 4] = data;
			tagMem[addrVals.set][assoc - 1] = addrVals.tag;
	}
	else {
		// There is an empty line. Write to it.
		cacheMem[addrVals.set][line][addrVals.offset / 4] = data;
		tagMem[addrVals.set][line] = addrVals.tag;
	}
}

void CacheSys::read(unsigned address) {
	// addresstoCache(address);
	AddressConverter addrVals(lineSize, sets, address);
	int line = tagCheck(addrVals.tag, addrVals.set);

	cout << setw(10) << address << setw(15) << mainMem[address / 4] << setw(15);
	if (line == -1) {
		cout << "none found" << endl;
		// Cache miss.
		// Copy value at address in main memory to the cache.
		this->write(address, mainMem[address / 4]);
	}
	else {
		// Line at address was seen in cache memory. Output it.
		cout << cacheMem[addrVals.set][line][addrVals.offset / 4] << endl;
	}
}

CacheSys::~CacheSys() {
	// Free your memory or the computer goes boom
	delete[] cacheMem;
	delete[] mainMem;
	delete[] tagMem;
}

int main() {
	ifstream in;
	in.open("input.txt");
	char inChar;
	unsigned lineSize; // Cache line size
	unsigned cacheSize; // Total cache size
	unsigned assoc; // Set associativity
	unsigned memSize; // Total main memory size
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
				die("Invalid value " + to_string(lineSize) + " for Line Size.");
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
	// Now we intialize our memory system by instantiating our CacheSys class
	CacheSys cacheSys(memSize, assoc, lineSize, cacheSize);
	// The processing loop
	cout << left << setw(10) << "Address" << setw(15) << "Data in Main" << "Data in Cache" << endl;
	while (!in.eof()) {
		unsigned address; // Gets the memory address passed by input file
		int writeData; // Treated as a char array holding our data (in bytes)
		in >> inChar;
		switch (inChar) {
		case 'E':
			in >> address;
			in >> inChar;
			if (inChar == 'W') {
				in >> writeData;
				cacheSys.write(address, writeData);
			}
			break;
		case 'F':
			// Display the address, value in the simulated cache, and value in the simulated main memory.
			in >> address;
			cacheSys.read(address);
			break;
		}
	}

	in.close();
	system("pause");
	return 0;
}

void die(const string & e) {
	cout << e << endl;
	system("pause");
	exit(EXIT_FAILURE);
}

unsigned bitCount(unsigned val) {
	return ceil(log(val) / log(2)); // Calculate # of bits required to hold a value
}