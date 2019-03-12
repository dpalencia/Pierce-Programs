#include <iostream>
#include <iomanip> // Setwidth
using namespace std;
struct S {
	double x;
	int n;
	char c;
};
bool sbb0(const S & a, const S & b);
bool sbb1(const S & a, const S & b);
void heapify(S bt[], unsigned els, bool(*sbb)(const S & a, const S & b));
bool demote(unsigned index, S bt[], unsigned els, bool(*sbb)(const S & a, const S & b));
void show(const S ar[], unsigned els);
ostream & operator<<(ostream & o, const S & s);
int main() {
	S tree[6] = {
		{0, 0, '0'}, // This element is an unused placeholder; we will begin our tree at index 1.
		{3.1414965358979, 3, '3'},
		{1.337, 1, 'h'},
		{2.718281828459045, 2, 'e'},
		{1.61803398875, 1000, 'g'},
		{.00000001, -200, '5'}
	};
	show(tree, 5);
	// heapify(tree, 6, sbb0); // Heapify with our first sort
	// heapify(tree, 6, sbb1); // Heapify with our second sort
	system("pause");
	return 0;
}
bool sbb0(const S & a, const S & b) {
	if (a.x != b.x)
		return a.x < b.x; // A goes first if the value of x is smaller
	if (a.n != b.n)
		return a.n < b.n;
	if ((48 <= a.c <= 57) != (48 <= b.c <= 57)) { // If one is a digit and one is not, the struct with the digit goes first (true means a should go before b)
		if ((48 <= a.c <= 57))
			return 1;
		return 0;
	}
	return a.c < b.c; // Char with the smaller ASCII code goes first
}
bool sbb1(const S & a, const S & b) {
	if (a.x != b.x)
		return a.x > b.x; // A goes first if the value of x is larger
	if (a.n != b.n)
		return a.n < b.n;
	if ((48 <= a.c <= 57) != (48 <= b.c <= 57)) { // The struct that is not the digit goes first (true means a should go before b)
		if (!(48 <= a.c <= 57))
			return 1;
		return 0;
	}
	return a.c < b.c; // Char with the smaller ASCII code goes first
}
void heapify(S bt[], unsigned els, bool(*sbb)(const S & a, const S & b)) {
	// Recursive swaps are needed
	// Check both children, and keep swapping until both swap functions return false (no swaps happened)
	// At that point we should have a full binary tree
	 
}
bool demote(unsigned index, S bt[], unsigned els, bool(*sbb)(const S & a, const S & b)) {
	// Check the base case (we're a leaf)
	// Identify the higher priority node
	// If this node is higher priority than current node, swap and return
	if (index * 2 > els) // Check the base case (we're a leaf)
		return false;
	unsigned hpni; // higher priority node index
	// Identify the higher priority node here 
	// Do we need a separate function for this? 
	
}
void show(const S ar[], unsigned els) { // We begin at index 1 because we are working with a 1-indexed tree
	cout << "Here is a tree: " << endl
		<< left << setw(8) << "Index" 
		<< setw(12) << "Double"
		<< setw(12) << "Int"
		<< setw(8) << "Char"
		<< endl;
	for (unsigned i = 1; i <= els; i++) {
		cout << setw(8) << i << ar[i] << endl;
	}
}
ostream & operator<<(ostream & o, const S & s) { // Operator overload to clean up show function
	o << left << setw(12) << s.x 
      << setw(12) <<  s.n 
      << setw(8) << s.c;
	return o;
}