// Palencia, Daniel
// 3-6-19
// CS 542 Discrete Structures
// Homework 4 Sets of Ordered Pairs
#include <iostream>
#include <set>
#include <assert.h>
using namespace std;
typedef pair<unsigned, unsigned> OP;
typedef set<pair<unsigned, unsigned>> SOP;
void show(const OP & op);
void show(const SOP & sop);
OP makeOP(unsigned first, unsigned second);
OP add(const OP & a, const OP & b);
bool elementOf(const OP & op, const SOP & sop);
bool firstFound(unsigned first, const SOP & sop);
bool secondFound(unsigned second, const SOP & sop);
bool eitherFound(unsigned n, const SOP & sop);
SOP Union(const SOP & sop0, const SOP & sop1);
int main() {
	OP p1(1000, 2000); // Using the constructor of OP
	show(p1); cout << "\n";
	OP p2 = makeOP(1337, 2112);
	show(p2); cout << "\n";
	cout << "Adding these two OP's gives:\n";
	OP p3 = add(p1, p2);
	show(p3); cout << "\n";
	SOP s = { p1, p2, p3 };
	show(s); cout << "\n";
	assert(elementOf(p1, s)); // Program explodes if elementOf tells us p1 is not in S
	assert(elementOf({ 1337, 2112 }, s)); // Using a literal to search for this ordered pair
										  // assert(elementOf(OP(1, 2), SOP{ OP(1,1), OP(2,1), OP(2,2) })); // This assert does, in fact, cause the program to explode
	assert(firstFound(1337, s)); // Should be OK because s contains the pair (1337, 2112)
	assert(secondFound(2112, s)); // Should be OK because s contains the pair (1337, 2112)
	assert(eitherFound(2112, s)); // Should be OK
	assert(eitherFound(1000, s)); // Should also be OK
								  // assert(eitherFound(12345, s)); <---- Definitely not OK
	SOP s2 = { OP(45, 21), OP(10000, 20000), OP(123, 456), OP(111, 222), OP(333, 444) }; // Create another set
	show(s2);
	cout << "\nLet's show the union of the two above sets \n";
	show(Union(s, s2)); // Show the union of the two sets
	cout << "\n";
	system("pause");
	return 0;
}
void show(const OP & op) {
	cout << "( " << op.first << ", " << op.second << " )";
}

void show(const SOP & sop) {
	cout << "-------- Showing set from show function ----------\n";
	for (auto it = sop.begin(); it != sop.end(); it++) {
		show(*it); cout << "\n";
	}
	cout << "------------------ End set ------------------";
}

OP makeOP(unsigned first, unsigned second) {
	return OP(first, second); // Returning an unnamed object.
							  // Can also assign parameters to .first and .second member variables on an instance of OP.
}

OP add(const OP & a, const OP & b) {
	return OP(a.first + b.first, a.second + b.second);
}

bool elementOf(const OP & op, const SOP & sop) {
	return sop.count(op);
}

bool firstFound(unsigned first, const SOP & sop) {
	for (auto it = sop.begin(); it != sop.end(); it++) {
		if ((*it).first == first)
			return true;
	}
	return false;
}

bool secondFound(unsigned second, const SOP & sop) {
	for (auto it = sop.begin(); it != sop.end(); it++) {
		if ((*it).second == second)
			return true;
	}
	return false;
}

bool eitherFound(unsigned n, const SOP & sop) {
	for (auto it = sop.begin(); it != sop.end(); it++) {
		if ((*it).first == n || (*it).second == n)
			return true;
	}
	return false;
}

SOP Union(const SOP & sop0, const SOP & sop1) {
	SOP ret;
	for (auto it = sop0.begin(); it != sop0.end(); it++)
		ret.insert(*it);
	for (auto it = sop1.begin(); it != sop1.end(); it++)
		ret.insert(*it);
	return ret;
}