// Palencia, Daniel
// 3-14-19
// CS 532 Lab Assignment 6 Pair and Set
#include <utility> // Pair
#include <set>
#include <iostream>
using namespace std;
typedef pair<unsigned, unsigned> PI; // Pair of ints
typedef set<PI> SPI; // Set of ordered pairs of ints
void show(const PI & p);
void show(const SPI & spi);
int main() {
	PI pArr[100]; // Array of 100 ordered pairs of ints
	SPI sPI; // Set of ordered pairs of ints
	for (unsigned i = 0; i < 100; i++) {
		pArr[i] = PI(i, pow(i, 3));
		sPI.insert(pArr[i]); // Push each pair into the set
	}
	PI p(0, 0); // The sum pair
	for (PI pi : pArr) {
		p.first += pi.first;
		p.second += pi.second;
	}
	// Show the sum of the firsts and the seconds
	cout << "Showing sum: "; show(p); cout << endl;
	sPI.insert(p); // Push the "sum" pair into the set
	for (auto it = sPI.begin(); it != sPI.end();) {
		if ((((*it).first % 7) == 0) && (((*it).second % 7) == 0)) {
			cout << "Removing: "; show((*it)); cout << endl;
			auto removed = it;
			it++;
			sPI.erase(removed);
		}
		else
			it++;
	}
	show(sPI);
	system("pause");
	return 0;
}
void show(const PI & p) {
	cout << "( " << p.first << ", " << p.second << " )";
}
void show(const SPI & spi) {
	for (PI p : spi) {
		show(p); cout << endl;
	}
}