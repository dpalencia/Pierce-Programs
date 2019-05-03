#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
using namespace std;
void die(const string & e);
unsigned bitCount(unsigned val);

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

class CacheSys { // Includes dynamically allocated cache memory and associative memory for tags
public:
	int** mainMem; // Line -> Offset
	Set* cacheMem; // Set -> Line -> Offset
	int** tagMem; // Set -> Line
	unsigned lineSize;
	unsigned memSize;
	unsigned assoc;
	unsigned sets;
	unsigned cacheSize;
	unsigned tag;
	unsigned set;
	int line;
	unsigned offset;
	void addresstoCache(unsigned address);
	CacheSys(unsigned inMemSize, unsigned inAssoc, unsigned inLineSize, unsigned inCacheSize); // Creates cache object and intializes cache memory.
	~CacheSys(); // Deletes dynamically allocated memory.
	void write(unsigned address, int data);
	unsigned cacheToMem();
	void read(unsigned address);
	bool tagCheck(unsigned address);
};

CacheSys::CacheSys(unsigned inMemSize, unsigned inAssoc, unsigned inLineSize, unsigned inCacheSize) {
	memSize = inMemSize;
	assoc = inAssoc;
	lineSize = inLineSize;
	cacheSize = inCacheSize;
	sets = cacheSize / lineSize / assoc;
	unsigned lineCount = memSize / lineSize;
	mainMem = nullptr;
	cacheMem = nullptr;
	tagMem = nullptr;
	try {
		mainMem = new int*[lineCount];
		for (unsigned i = 0; i < lineCount; i++) {
			mainMem[i] = new int[lineSize / 4];
		};
		cacheMem = new Set[sets];
		tagMem = new int*[sets];
		for (unsigned i = 0; i < sets; i++) {
			cacheMem[i] = Set(lineSize, assoc);
			tagMem[i] = new int[assoc];
		}
	}
	catch (bad_alloc &) {
		die("could not allocate memory.");
	}
	// Initialize everything to -1.
	for (unsigned i = 0; i < lineCount; i++) {
		for (unsigned j = 0; j < lineSize / 4; j++) {
			mainMem[i][j] = -1;
		}
	}
	for (unsigned i = 0; i < sets; i++) {
		for (unsigned k = 0; k < assoc; k++) {
			tagMem[i][k] = -1;
		}
	}

}

void CacheSys::addresstoCache(unsigned address) {
	tag = set = offset = 0;
	offset = (address & (lineSize - 1)) / 4;
	address >>= bitCount(lineSize);
	set = (address & (sets - 1));
	address >>= bitCount(sets);
	tag = address;
}

unsigned CacheSys::cacheToMem() {
	unsigned ret = 0;
	ret |= tag;
	ret <<= (bitCount(sets));
	ret |= set;
	ret <<= (bitCount(lineSize));
	return ret / lineSize / 4; // Divide by 4 to get memory index. (Assuming 4 byte words).
} 

bool CacheSys::tagCheck(unsigned address) {
	// Check tag against address and set line that contains it
	for (unsigned i = 0; i < assoc; i++) {
		if (tagMem[set][i] == tag) {
			unsigned addressFromMem = tag;
			addressFromMem <<= bitCount(sets);
			addressFromMem |= set;
			addressFromMem <<= (bitCount(memSize) - bitCount(sets));
			if ((address - address % lineSize) == addressFromMem) {
				line = i;
				return 1;
			}
		}
	}
	return 0;
}

void CacheSys::write(unsigned address, int data) {
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
}

void CacheSys::read(unsigned address) {
	addresstoCache(address);
	tagCheck(address);
	cout << "Address: " << address << " Memory: " << mainMem[address / sets][offset] << " Cache: " 
		<< cacheMem[set].getWord(line, offset) << endl;
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