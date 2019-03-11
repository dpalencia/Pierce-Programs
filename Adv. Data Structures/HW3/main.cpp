// Palencia, Daniel
// 3-6-19
// CS 532 Advanced Data Structures
// Homework 3 Huffman Encode and Decode
#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <iomanip>
using namespace std;
void die(const string & e); // Die helper function
string getBuffer(const string & inputFile); // This helper function will read the input file and return a "buffer" that contains the contents of the text
void getFreq(unsigned long long freq[256], const string & buffer); // Read from buffer and change freq so that freq[n] = freq of char with ascii code N
void compressWrite(const string code[256], const string & buffer, const string & outputFile); // Reads the input file and writes the code to the output file
ifstream ifOpenBin(const string & fileName); // Opens input files in binary mode
ofstream ofOpenBin(const string & fileName); // Opens output files in binary mode
class Node {
public:
	Node * _left;
	Node* _right;
	unsigned char _c;
	unsigned long long _freq;
	Node(char c = '0', unsigned long long freq = 0, Node* left = nullptr, Node* right = nullptr) :
		_c(c), _freq(freq), _left(left), _right(right) {};
	static Node* newNode(char c, unsigned long long freq); // Static so we can call the function stand-alone
	bool operator() (Node* n1, Node* n2) { // Overload () operator so we can use it in the priority_queue comparison parameter
		return n1->_freq > n2->_freq;
	}
};
void decode(const string & inputFile, const string & outputFile, const Node* tree);
void deallocate(Node* p); // Helper function to deallocate the nodes in the tree from DMA in huffman function
void traverse(const Node* p, ofstream & o, const string & buffer); // Traverse except this time we return the char contained in the leaf for decoding purposes
void traverse(string code[256], Node* p); // Recursive function to traverse huffman tree and encode
void traverse(string code[256], Node* p, string temp, char b);
Node* huffman(string code[256], const unsigned long long freq[256]);
int main() {
	string code[256], inputFile, littleOut, bigOut, buffer;
	unsigned long long f[256];
	cout << "Enter the name of your input file." << endl;
	getline(cin, inputFile);
	cout << "Enter the name of the file to which we will write the compressed input file. " << endl;
	getline(cin, littleOut);
	cout << "Enter the name of the file to which we will write the decoded compressed file. " << endl;
	getline(cin, bigOut);
	buffer = getBuffer(inputFile); // "Buffer" is a string that contains the contents of the input file
	getFreq(f, buffer); // Get the frequency of each char and write it to our "f" array.
	Node* huff = huffman(code, f); // Build the huffman code from our freq values and store it in the code string; return a pointer to the tree root
	compressWrite(code, buffer, littleOut); // Read from the buffer and write the huffman encoded chars to the "little" output file.
	decode(littleOut, bigOut, huff); // Decode the compressed file and write it to the big out file
	system("pause");
	return 0;
}

Node* Node::newNode(char c, unsigned long long freq) { // Create a new node and return a pointer to it
	Node* p = nullptr;
	try {
		p = new Node(c, freq);
	}
	catch (bad_alloc &) {
		die("could not allocate memory.");
	}
	return p;
};

void getFreq(unsigned long long freq[256], const string & buffer) {
	for (unsigned i = 0; i < 256; i++) { // Initialize freq array
		freq[i] = 0;
	}
	for (unsigned i = 0; i < buffer.size(); i++) { // Read through buffer string and increment freq[i] where is i is the hex value of the char
		if (buffer.at(i) < 0) // Handle the case where the char is < 0 to get appropriate array index for ASCII value
			freq[buffer.at(i) + 256]++;
		else
			freq[buffer.at(i)]++;
	}
}

Node* huffman(string code[256], const unsigned long long freq[256]) {
	// Use the priority queue to push and pop nodes
	// Using a vector of pointers to nodes so that I can dynamically allocate nodes,
	// and assign these new pointers to my nodes
	priority_queue<Node*, vector<Node*>, Node> q;
	for (unsigned i = 0; i < 256; i++) { // Push a DMA leaf for every char that appears at least once
		if (freq[i] > 0) {
			q.push(Node::newNode(i, freq[i]));
		}
	}
	if (q.size() == 0)
		die("no data."); // Die if we don't have any data to work with (such as if we have an empty input file)
	Node* root = q.top();
	while (q.size() > 1) {
		Node* n1 = q.top(); q.pop();
		Node* n2 = q.top(); q.pop();
		root = Node::newNode('0', n1->_freq + n2->_freq);
		root->_left = n1;
		root->_right = n2;
		q.push(root);
	}
	traverse(code, root); // Traverse will assign the huffman code for each char, and store it into the "code" array
	return root;
}

void traverse(string code[256], Node* p) {
	string temp; // Create a temp string that will be copied into each recursive function call
	if (p->_left != nullptr && p->_right != nullptr) {
		traverse(code, p->_left, temp, '0');
		traverse(code, p->_right, temp, '1');
		return;
	}
	// If we make it this far, that means we only have a single leaf. Assign its code to be 0.
	if (p->_c < 0) {
		code[p->_c + 256] = '0';
		return;
	}
	code[p->_c] = '0';
}

void traverse(string code[256], Node* p, string temp, char b) {
	// Append each copy of the string with the bit value we pass into the function
	temp += b;
	// Check if the current node is a leaf (a leaf will  have nullptr on both of its pointers)
	if (p->_left != nullptr && p->_right != nullptr) {
		traverse(code, p->_left, temp, '0');
		traverse(code, p->_right, temp, '1');
		return;
	}
	// If we make it this far, assign the code to the corresponding character index in our code array
	if (p->_c < 0) { // Char is typecast as an int. Check if the char is a negative int value before using it as an array index
		code[p->_c + 256] = temp; // Add 256 if it is a negative ascii value, so that it is a valid array index
		return;
	}
	code[p->_c] = temp;
}

void traverse(const Node* p, ofstream & o, const string & buffer) { // Recursively traverse tree and write to the output file
	static unsigned bitsRemaining = 8;
	static unsigned i = 0;
	if (p->_left == nullptr && p->_right == nullptr) {
		o.put(p->_c);
		return;
	}
	if (bitsRemaining == 0) {
		bitsRemaining = 8;
		i++;
	}
	if ((buffer[i] >> (bitsRemaining - 1)) & 1) { // If the current bit is 1, traverse right
		bitsRemaining--;
		traverse(p->_right, o, buffer);
	}
	else {
		bitsRemaining--;
		traverse(p->_left, o, buffer); // If the current bit is 0, traverse left
	}
}
void deallocate(Node* p) {	// Recursive memory deallocation
	if (p->_left != nullptr && p->_right != nullptr) {
		deallocate(p->_left);
		deallocate(p->_right);
	}
	delete p;
}
void compressWrite(const string code[256], const string & buffer, const string & outputFile) { // Reads the input file and writes the code to the "little" output file
	ofstream out = ofOpenBin(outputFile);
	string carr; // This will hold the huffman-encoded counterpart of the buffer
	for (unsigned i = 0; i < buffer.size(); i++) { // Iterate through buffer and store the huffman encoding into a string
		if (buffer.at(i) < 0)
			carr += code[buffer.at(i) + 256];
		else
			carr += code[buffer.at(i)];
	}
	// Write the number of chars in the output file as an unsigned long long
	unsigned long long charCount = buffer.size(); // Get the char count 
	unsigned long long* countPtr = &charCount; // We need a pointer to our char count to pass it to memcpy
	char lengthBytes[8]; // Pointer to a fixed block of 8 char bytes
	memcpy(lengthBytes, countPtr, sizeof(*countPtr)); // Copy the memory in countPtr to lengthBytes ptr
	out.write(lengthBytes, 8);
	unsigned remainingBits = 8; // Count how many bits we have as we write bits to the byte variable
	char byte = 0; // This will be the 1 byte variable we will bit manipulate
	cout << "Writing compressed data to " << outputFile << "..." << endl;
	for (unsigned i = 0; i < carr.size(); i++) {
		if (remainingBits == 0) {
			out.put(byte);
			byte = 0; // Flush the byte
			remainingBits = 8; // Reset remaining bits count
		}
		byte <<= 1;
		if (carr.at(i) == '1')
			byte |= 1; // Put the current bit into our byte
		remainingBits--;
	}
	byte <<= remainingBits; // Push the written bits to the left of the byte
	out.put(byte); // Write the last byte
	out.close();
	cout << "Compression complete!" << endl;
}

void decode(const string & inputFile, const string & outputFile, const Node* tree) {
	ifstream in = ifOpenBin(inputFile);
	ofstream out = ofOpenBin(outputFile);
	string buffer; // This string will store each byte in the input file
	char lengthBytes[8]; // Read the first 8 bytes of the file and get its length. Needs to be pointer so we can use the memcpy function.
	in.read(lengthBytes, 8);
	unsigned long long charCount;
	unsigned long long* countPtr = &charCount;
	memcpy(countPtr, lengthBytes, sizeof(lengthBytes));
	char c;
	while (in.get(c)) {
		buffer += c;
	}
	if (!in.eof())
		die("there was an error reading the file.");
	cout << "Decompressing data from " << inputFile << " to " << outputFile << "..." << endl;
	for (unsigned i = 0; i < charCount; i++) {
		traverse(tree, out, buffer);
	}
	in.close();
	out.close();
	cout << "Decompression complete!" << endl;
}

string getBuffer(const string & inputFile) { // Read a file and get the string buffer for input
	string buffer;
	char c;
	ifstream in = ifOpenBin(inputFile);
	while (in.get(c)) { // Read the file and put each char in the buffer string
		buffer += c;
	}
	if (!in.eof()) // Check if EOF is OK
		die("there was an error reading the file.");
	in.close();
	return buffer;
}

ifstream ifOpenBin(const string & fileName) { // Helper function that opens input files in binary mode
	ifstream f;
	f.open(fileName, ios::binary);
	if (!f)
		die("could not open file.");
	return f;
}

ofstream ofOpenBin(const string & fileName) { // Helper function that opens output files in binary mode
	ofstream f;
	f.open(fileName, ios::binary);
	if (!f)
		die("could not open file");
	return f;
}

void die(const string & e) {
	cout << "ERROR: " << e << endl;
	system("pause");
	exit(EXIT_FAILURE);
}