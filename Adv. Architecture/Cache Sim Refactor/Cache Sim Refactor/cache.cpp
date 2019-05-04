#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
using namespace std;
void die(const string & e);
unsigned bitCount(unsigned val);
/*
class Set {
public:
	Set() {}
	Set(unsigned lineSize, unsigned assoc);
	int emptyLine(unsigned lineSize, unsigned assoc);
	int** set; // Contains all of the lines in the set indexed by [line][offset]
	void writeWord(unsigned line, unsigned offset, int data);
	int getWord(unsigned line, unsigned offset); // Fetches a word from the current set.
	void evictLine(unsigned lineSize, unsigned assoc);
};

Set::Set(unsigned lineSize, unsigned assoc) {
	set = nullptr;
	try {
		set = new int*[assoc * (lineSize / 4)];
		for (unsigned i = 0; i < assoc; i++) {
			set[i] = new int[lineSize / 4];
		}
	}
	catch (bad_alloc & e) {
		die("could not allocate memory.");
	}
	for (unsigned i = 0; i < assoc; i++) {
		for (unsigned j = 0; j < lineSize / 4; j++) {
			set[i][j] = -1;
		}
	}
};

void Set::writeWord(unsigned line, unsigned offset, int data) {
	set[line][offset] = data;
} 

int Set::getWord(unsigned line, unsigned offset) {
	return set[line][offset];
}

int Set::emptyLine(unsigned lineSize, unsigned assoc) {
	// Check if the set contains an empty line
	unsigned emptyCount;
	for (unsigned i = 0; i < assoc; i++) {
		emptyCount = 0;
		for (unsigned j = 0; j < lineSize / 4; j++) {
			if (set[i][j] == -1)
				emptyCount++;
		}
		if (emptyCount == (lineSize / 4)) {
			return i;
		}
	}
	return -1;
}

void Set::evictLine(unsigned lineSize, unsigned assoc) {
	// We could do this as a queue, the pointer way where each "line" is a node with a head and tail if we really wanted to
	set[assoc - 1][(lineSize / 4) - 1] = -1;
	for (unsigned i = 1; i < assoc; i++) {
		for (unsigned j = 1; j < lineSize / 4; j++) {
			set[i][j] = set[i - 1][j - 1];
		}
	}
}
*/

class AddressConverter { 
// This class either does these conversions depending on input:
	// (lineSize, number of sets, address) -> Tag, Set, Offset
	// (Tag, Set, Offset) -> Address
// Or it takes a a tag, set, and offset and gives back a memory address.
public:
	unsigned tag, set, offset, address;
	AddressConverter(unsigned inOffset, unsigned lineSize, unsigned setCount, unsigned inAddress) {
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
	int* mainMem; // Line -> Offset
	int*** cacheMem; // Set -> Line -> Offset
	int** tagMem; // Set -> Line
	unsigned lineSize;
	unsigned memSize;
	unsigned assoc;
	unsigned sets;
	unsigned cacheSize;
	/* unsigned tag;
	unsigned set;
	int line;
	*/
	unsigned offset;
	//void addresstoCache(unsigned address);
	CacheSys(unsigned inMemSize, unsigned inAssoc, unsigned inLineSize, unsigned inCacheSize); // Creates cache object and intializes cache memory.
	~CacheSys(); // Deletes dynamically allocated memory.
	void write(unsigned address, int data);
	//unsigned cacheToMem();
	void read(unsigned address);
	int tagCheck(unsigned tag, unsigned set);
};

CacheSys::CacheSys(unsigned inMemSize, unsigned inAssoc, unsigned inLineSize, unsigned inCacheSize) {
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
/*
void CacheSys::addresstoCache(unsigned address) {
	tag = set = offset = 0;
	offset = (address & (lineSize - 1)) / 4;
	address >>= bitCount(lineSize);
	set = (address & (sets - 1));
	address >>= bitCount(sets);
	tag = address;
}
*/
/*
unsigned CacheSys::cacheToMem() {
	unsigned ret = 0;
	ret |= tag;
	ret <<= (bitCount(sets));
	ret |= set;
	ret <<= (bitCount(lineSize));
	return ret / lineSize / 4; // Divide by 4 to get memory index. (Assuming 4 byte words).
} 
*/

int CacheSys::tagCheck(unsigned tag, unsigned set) {
	// Check tag against address and set line that contains it, or -1 if the line is not found in the cache.
	for (unsigned i = 0; i < assoc; i++) {
		if (tagMem[set][i] == tag) 
				return i;
	}
	return -1;
}

void CacheSys::write(unsigned address, int data) {
	AddressConverter addrVals(offset, lineSize, sets, address);
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
				mainMem[cacheVals.address + i] = cacheMem[addrVals.set][0][i];
			}
			// Shift every line in the cache back by one
			for (unsigned i = 0; i < assoc - 1; i++) {
				for (unsigned j = 0; j < (lineSize / 4) - 1; j++) {
					cacheMem[addrVals.set][i][j] = cacheMem[addrVals.set][i + 1][j];
				}
				tagMem[addrVals.set][i] = tagMem[addrVals.set][i + 1];
			}
			// Write to the last line in the cache.
			//delete cacheMem[addrVals.set][assoc - 1];
			cacheMem[addrVals.set][assoc - 1][addrVals.offset / 4] = data;
			tagMem[addrVals.set][assoc - 1] = addrVals.tag;
	}
	else {
		// There is an empty line. Write to it.
		cacheMem[addrVals.set][line][addrVals.offset / 4] = data;
		tagMem[addrVals.set][line] = addrVals.tag;
	}

	/*
	addresstoCache(address);
	// First check if the address is already in the cache and write to it if it is
	if (tagCheck(address)) {
		cacheMem[set].writeWord(line, offset, data);
		return;
	}
	// Now see if there is empty line to write to.
	line = cacheMem[set].emptyLine(lineSize, assoc);
	if (line == -1) { // Our set is full. Evict the last line from the set, then write to the first line.
		unsigned memIndex = cacheToMem();
		for (unsigned i = 0; i < lineSize / 4; i++) { // Get the contents of the last line in the set.
			mainMem[memIndex][i] = cacheMem[set].getWord(0, i); // Write each word to the corresponding memory location.
		 }
		cacheMem[set].evictLine(lineSize, assoc);
		for (unsigned i = 1; i < assoc; i++) {
			tagMem[set][i] = tagMem[set][i - 1]; // Update tags. Shifted up by one.
		}
		cacheMem[set].writeWord(0, offset, data);
		tagMem[set][0] = tag;
	}
	else { // Otherwise we have a nice, empty cache line to write directly to. 
		cacheMem[set].writeWord(line, offset, data);
		tagMem[set][line] = tag;
	}
	*/
}

void CacheSys::read(unsigned address) {
	// addresstoCache(address);
	AddressConverter addrVals(offset, lineSize, sets, address);
	int line = tagCheck(addrVals.tag, addrVals.set);
	cout << "Address: " << address << " Memory: " << mainMem[address / 4] << " Cache: ";
	if (line == -1) {
		cout << "memory block not found in cache." << endl;
	}
	else {
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
	// Now we intialize our memory system by instantiating an object.
	CacheSys cacheSys(memSize, assoc, lineSize, cacheSize);
	// The processing loop
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