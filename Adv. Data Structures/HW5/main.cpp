#include <iostream>
using namespace std;
struct S {
	double x;
	int n;
	char c;
};
bool sbb0(const S & a, const S & b);
bool sbb1(const S & a, const S & b);
void heapify(S bt[], unsigned els, bool(*sbb)(const S & a, const S & b));
int main() {
	S tree[6] = {
		{0, 0, '0'}, // This element is an unused placeholder; we will begin our tree at index 1.
		{3.1414965358979, 3, '3'},
		{1.337, 1, 'h'},
		{2.718281828459045, 2, 'e'},
		{1.61803398875, 1000, 'g'},
		{.00000001, -200, '5'}
	};
	heapify(tree, 6, sbb0); // Heapify with our first sort
	heapify(tree, 6, sbb1); // Heapify with our second sort
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
	// Recursive swaps may be needed
	// Write a recursive swap function?
}