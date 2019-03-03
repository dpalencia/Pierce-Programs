// Palencia, Daniel
// 2-27-19
// CS 532 Advanced Data Structures
// Homework 1 Huffman Code
#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <iomanip>
using namespace std;
void die(const string & e); // Die helper function
class Node {
public:
	Node * _left;
	Node* _right;
	char _c;
	unsigned long long _freq;
	Node(char c = '0', unsigned long long freq = 0, Node* left = nullptr, Node* right = nullptr) :
		_c(c), _freq(freq), _left(left), _right(right) {};
	static Node* newNode(char c, unsigned long long freq); // Static so we can call the function stand-alone
	bool operator() (Node* n1, Node* n2) { // Overload () operator so we can use it in the priority_queue comparison parameter
		return n1->_freq > n2->_freq;
	}
};

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

string getBuffer(const string & inputFile); // This helper function will read the input file and return a "buffer" that contains the contents of the text
void getFreq(unsigned long long freq[256], const string & buffer); // Read from buffer and change freq so that freq[n] = freq of char with ascii code N
void huffman(string code[256], const unsigned long long freq[256]);
void traverse(string code[256], Node* p); // Recursive function to traverse huffman tree
void traverse(string code[256], Node* p, string temp, char b);
void deallocate(Node* p); // Helper function to deallocate the nodes in the tree from DMA in huffman function
void compressWrite(const string code[256], const string & buffer, const string & outputFile); // Reads the input file and writes the code to the output file
ifstream ifOpenBin(const string & fileName); // Opens input files in binary mode
ofstream ofOpenBin(const string & fileName); // Opens output files in binary mode

int main() {
	string code[256], inputFile, littleOut, bigOut, buffer;
	unsigned long long f[256];	
	cout << "Enter the name of your input file." << endl;
	getline(cin, inputFile);
	cout << "Enter the name of the file to which we will write the compressed input file. " << endl;
	getline(cin, littleOut);
	cout << "Enter the name of the file to which we will write the decoded compressed file. " << endl;
	getline(cin, bigOut);
	buffer = getBuffer(inputFile);
	getFreq(f, buffer); // Get the frequency of each char and write it to our "f" array.
	huffman(code, f); // Build the huffman code from our freq values and store it in the code string
	compressWrite(code, buffer, littleOut); // Read from the input file and write the huffman encoded chars to the "little" output file.
	system("pause");
	return 0;
}

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

void huffman(string code[256], const unsigned long long freq[256]) {
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
	cout << left << setw(9) << "ASCII #" << setw(11) << "Freq" << setw(40) << "Code" << endl;
	for (int i = 0; i < 128; i++) {
		if (freq[i] > 0)
			cout << setw(9) << i << setw(11) << freq[i] << setw(40) << code[i] << endl;
	}
	for (int i = -128; i < 0; i++) {
		if (freq[i + 256] > 0)
			cout << setw(9) << i + 256 << setw(11) << freq[i + 256] << setw(40) << code[i + 256] << endl;
	}
	deallocate(root);
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

void deallocate(Node* p) {	// Recursive memory deallocation
	if (p->_left != nullptr && p->_right != nullptr) {
		deallocate(p->_left);
		deallocate(p->_right);
	}
	delete p;
}

void compressWrite(const string code[256], const string & buffer, const string & outputFile) { // Reads the input file and writes the code to the "little" output file
	ofstream out = ofOpenBin(outputFile);
	for (unsigned i = 0; i < buffer.size(); i++) // Iterate through buffer and write the code for eachc corresponding char to the output file.
		if (buffer.at(i) < 0)
			out << code[buffer.at(i) + 256];
		else
			out << code[buffer.at(i)];
	out.close();
} 

string getBuffer(const string & inputFile) {
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