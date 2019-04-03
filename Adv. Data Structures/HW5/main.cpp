// Palencia, Daniel
// 3-13-19
// CS 532
// Homework 5 Heaps
#include <iostream>
#include <iomanip> // Setwidth
using namespace std;
struct S {
	double x;
	int n;
	char c;
};
bool sbb0(const S & a, const S & b);
bool sbb1(const S & a, const S & b); // Compares the int values to create a min heap
void heapify(S bt[], unsigned els, bool(*sbb)(const S & a, const S & b));
void demote(unsigned index, S bt[], unsigned els, bool(*sbb)(const S & a, const S & b));
bool demoteCheck(unsigned index, S bt[], unsigned els, bool(*sbb)(const S & a, const S & b));
void show(const S ar[], unsigned els);
S heapPop(S ar[], unsigned els, bool(*sbb)(const S & a, const S & b));
ostream & operator<<(ostream & o, const S & s);
int main() {
	S tree[11] = {
		{ 0, 0, '0' }, // This element is an unused placeholder; we will begin our tree at index 1.
	{ 3.141496, 9, '3' },
	{ 1.337, 8, 'h' },
	{ 2.7182818, 5, 'e' },
	{ 1.6185, 1, 'g' },
	{ .000001, 1000, '5' },
	{ 1.113, 123, 'r' },
	{ 100.011, 50, '%' },
	{ 93.62, 42, '^' },
	{ 12939, 10, 'Q' },
	{ 0.191928383, 10, 'v' }
	};
	// Show the unaltered tree
	cout << "THE UNSORTED ARRAY: " << endl;
	show(tree, 11);
	cout << endl;
	// Heapify the tree using sbb0 sorting, then output the contents as an array with our show function
	cout << "SBB0 SORTING: " << endl;
	heapify(tree, 11, sbb0);
	show(tree, 11);
	cout << endl;
	// Heapify the tree using sbb1 sorting
	cout << "---------Popping the entire heap as a min heap\n"
		<< "---------increasing order (comparing ints using sbb1)" << endl;
	heapify(tree, 11, sbb1);
	// Output the sorted heap, except this time we pop (remove) the root as we go
	for (unsigned i = 1; i < 11; i++)
		cout << heapPop(tree, 11, sbb1) << endl;
	// The heap is now full of garbage.
	cout << endl << "The heap is now full of garbage. It is a \"garbage\" heap." << endl;
	system("pause");
	return 0;
}
bool sbb0(const S & a, const S & b) {
	if (a.x != b.x)
		return a.x < b.x; // A goes first if the value of x is smaller
	if (a.n != b.n) // If the ints are not equal, return if a.n < b.n
		return a.n < b.n;
	if ((48 <= a.c <= 57) != (48 <= b.c <= 57)) { // If one is a digit and one is not, the struct with the digit goes first (true means a should go before b)
		if ((48 <= a.c <= 57))
			return 1;
		return 0;
	}
	return a.c < b.c; // Char with the smaller ASCII code goes first
}
bool sbb1(const S & a, const S & b) { // Simply returns if a.n < b.n
	return a.n < b.n;
}
void heapify(S bt[], unsigned els, bool(*sbb)(const S & a, const S & b)) {
	// Recursive swaps are needed
	// Check both children, and keep swapping until both demoteCheck functions return false (no demotes happened)
	// Keep traversing the tree and checking if we need to demote; as soon as no demotes happen, exit the loop and we have a heap
	while (demoteCheck(1, bt, els, sbb)) {}
}

bool demoteCheck(unsigned index, S bt[], unsigned els, bool(*sbb)(const S & a, const S & b)) {
	// Recursively look at every node of tree and see if demotes need to happen
	// Return false if no demotes need to happen
	// Return true if at least one demote happened
	// If we're at the end of the tree, no demotes needed to happen
	if (index * 2 > (els - 1))
		return false;
	if (sbb(bt[index * 2], bt[index])) { // Check the left child
		demote(index, bt, els, sbb);
		return true;
	}
	if ((index * 2 + 1) < els) { // Only check the right child if it is not out of bounds
		if (sbb(bt[(index * 2) + 1], bt[index])) {
			demote(index, bt, els, sbb);
			return true;
		}
	}
	return demoteCheck(index * 2, bt, els, sbb) || demoteCheck((index * 2) + 1, bt, els, sbb); // Keep going if no demotes happened
}

void demote(unsigned index, S bt[], unsigned els, bool(*sbb)(const S & a, const S & b)) {
	// Check the base case (we're a leaf)
	// Identify the higher priority node 
	// If this node is higher priority than current node, swap and return
	if (index * 2 > (els - 1)) // Check the base case (we're a leaf)
		return;
	unsigned hpni; // higher priority node index
				   // Identify the higher priority node 
	if (((index * 2) + 1) > (els - 1) || sbb(bt[index * 2], bt[(index * 2) + 1]))
		// Right child is out of bounds || Left Child is higher priority than Right Child
		hpni = index * 2;
	else
		hpni = (index * 2) + 1;
	// Now compare the current node with this higher priority node
	if (sbb(bt[hpni], bt[index])) {
		swap(bt[hpni], bt[index]);
	}
	demote(hpni, bt, els, sbb); // Keep swapping until the node is in the proper place
}

void swap(S* a, S* b) {
	S temp = *a;
	*a = *b;
	*b = temp;
}

void show(const S ar[], unsigned els) { // We begin at index 1 because we are working with a 1-indexed tree
	cout << "---HEAP-------------------------------------" << endl
		<< left << setw(8) << "Index"
		<< setw(12) << "Double"
		<< setw(12) << "Int"
		<< setw(8) << "Char"
		<< endl;
	for (unsigned i = 1; i < els; i++) {
		cout << setw(8) << i << ar[i] << endl;
	}
}
ostream & operator<<(ostream & o, const S & s) { // Operator overload to clean up show function
	o << left << setw(12) << s.x
		<< setw(12) << s.n
		<< setw(8) << s.c;
	return o;
}

S heapPop(S ar[], unsigned els, bool(*sbb)(const S & a, const S & b)) {
	static unsigned size = els - 1; // Keep track of the size of the heap as we keep popping
									// This might work better as a class if we wish to add more functions (push, insert)
	S ret = ar[1];
	for (unsigned i = 1; i < size; i++) {
		ar[i] = ar[i + 1];
	}
	size--;
	heapify(ar, size + 1, sbb); // Maintain the heap property as we go
	return ret;
}