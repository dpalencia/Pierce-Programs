// Probability.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <vector>
#include <cstdlib>
#include <stack> // For the parens
#define TRIALS 1000000
using namespace std;
double monotonic(unsigned a, unsigned b);
bool isMonotonic(const vector<int>& vi);
double strictlyMonotonic(unsigned a, unsigned b);
bool isStrictlyMonotonic(const vector<int>& vi);
double okNesting(unsigned n);
template <typename T> void shuffle(vector<T> & v);
int main() {
	// ---> Instead of building a vector every trial, create a vector once then shuffle it every trial. <----
	cout << "3 bit probability numbers: " << endl;
	cout << "Monotonic: " << monotonic(2, 3) << endl;
	cout << "Strictly monotonic: " << strictlyMonotonic(2, 3) << endl;
	cout << "% Chance of OK nesting, 10 sets of parens" << okNesting(10) << endl;
	return 0;
}

template <typename T> void shuffle(vector<T> & v) {
	for (unsigned i = 0; i < v.size(); i++) {
		unsigned randi = rand() % (v.size() - 1); // Random index
		T temp = v[randi]; // The 3-step swap
		v[randi] = v[i];
		v[i] = temp;
	}
}

double monotonic(unsigned a, unsigned b) {
	// First determine if it is monotonically increasing or decreasing
	// Push a number in the range [0, a) into a vector, b times.
	// Might not even need to construct a vector for this;
	// Will do it the vector way for now.
	vector<int> vi;
	for (unsigned i = 0; i < b; i++) {
		vi.push_back(rand() % a);
	}
	unsigned count = 0; // count of # of times the sequence was monotonic.
	for (unsigned i = 0; i < TRIALS; i++) {
		shuffle(vi);
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
	for (unsigned i = 0; i < b; i++) {
		vi.push_back(rand() % a);
	}
	unsigned count = 0;
	for (unsigned i = 0; i < TRIALS; i++) {
		shuffle(vi);
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

double okNesting(unsigned n) {
	unsigned count = 0; // Count the number of positives in our trials.
	// Check the probability of 
	// a sequence of n left parens and n right parens
	// being properly nested
	vector<char> vc; // First build our vector.
	for (unsigned i = 0; i < (n * 2); i++) {
		vc.push_back(rand() % 2 + 40);
	}
	stack<char> stc; // Stack of chars. Push onto stack if '(', pop if ')'
	for (unsigned i = 0; i < TRIALS; i++) {
		shuffle(vc);
		for (char c : vc) {
			if (c == 40)
				stc.push(c);
			if (c == 41) {
				if (stc.empty())  // Mismatched parens.
					break;
				stc.pop();
			}
 		}
		count++;
	}
	return double(count) / TRIALS;
}

