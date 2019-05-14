// Probability.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <vector>
#include <cstdlib>
#define TRIALS 1000000
using namespace std;
double monotonic(unsigned a, unsigned b);
bool isMonotonic(const vector<int>& vi);
double strictlyMonotonic(unsigned a, unsigned b);
bool isStrictlyMonotonic(const vector<int>& vi);
int main() {
	cout << "FIX THE MONOTONIC LOGIC!" << monotonic(2, 3) << endl;
	cout << "Probability that a sequence of 3 numbers in [0, 2) is stricly monotonic:\n" << strictlyMonotonic(2, 3) << endl;
	return 0;
}

double monotonic(unsigned a, unsigned b) {
	// First determine if it is monotonically increasing or decreasing
	// Push a number in the range [0, a) into a vector, b times.
	// Might not even need to construct a vector for this;
	// Will do it the vector way for now.
	vector<int> vi;
	unsigned count = 0; // count of # of times the sequence was monotonic.
	for (unsigned i = 0; i < TRIALS; i++) {
		vi.clear();
		for (unsigned i = 0; i < b; i++) {
			vi.push_back(rand() % a);
		}
		if (isMonotonic(vi)) count++;
	}
	return double(count) / TRIALS; // fraction of times the sequence was monotonic.
}

bool isMonotonic(const vector<int>& vi)  {
	if (vi.size() <= 2) return true; // A sequence of 2 or fewer numbers is always monotonic.
	unsigned i; // Initial index to check if it's monotonically increasing or decreasing,	
	bool increasing = 0;
	for (i = 0; (i < (vi.size() - 1)) && !increasing; i++) {
		if (vi[i + 1] > vi[i]) {
			increasing = 1;
			break;
		}
		if (vi[i + 1] < vi[i]) {
			break;
		}
	}
	if (increasing) {
		for (; i < (vi.size() - 1); i++) {
			if (vi[i + 1] < vi[i])
				return false;
		}
	} 
	else {  
		for (; i < (vi.size() - 1); i++) {
			if (vi[i + 1] > vi[i])
				return false;
		}
	}
	return true;
}

double strictlyMonotonic(unsigned a, unsigned b) {
	// Same as monotonic, except now we also disallow repeats of numbers.
	// Do it the vector way like before,
	vector<int> vi;
	unsigned count = 0;
	for (unsigned i = 0; i < TRIALS; i++) {
		vi.clear();
		for (unsigned i = 0; i < b; i++) {
			vi.push_back(rand() % a);
		}
		if (isStrictlyMonotonic(vi)) count++;
	}
	return double(count) / TRIALS;
}

bool isStrictlyMonotonic(const vector<int>& vi) {
	if (vi.size() < 2) return true; // A vector with one element or an empty vector will be strictly monotonic.
	bool increasing = 0;
	
	if (vi[0] == vi[1]) return false;
	if (vi[0] < vi[1]) increasing = 1;

	if (increasing) {
		for (unsigned i = 0; i < (vi.size() - 1); i++) {
			if (vi[i + 1] <= vi[i])
				return false;
		}
	}
	else {
		for (unsigned i = 0; i < (vi.size() - 1); i++) {
			if (vi[i + 1] >= vi[i])
				return false;
		}
	}
	return true;
}