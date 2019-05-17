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
double duel(double a, double b);
bool outcome(double probability);
double flip(double p); // Need to clarify on k heads.
double prettyLady(unsigned gridX, unsigned gridY, unsigned ladyX, unsigned ladyY);

int main() {
	cout << "3 bit probability numbers: " << endl;
	cout << "Monotonic: " << monotonic(2, 3) << endl;
	cout << "Strictly monotonic: " << strictlyMonotonic(2, 3) << endl;
	cout << "% Chance of OK nesting, 2 sets of parens\n" << okNesting(2) << endl;
	cout << "In a duel where both duellists hit 50% of the time and A shoots first:\n"
		 << "Probability duellist A wins: " << duel(.5, .5) << endl;
	cout << "In a 2x2 grid, chance of seeing the pretty lady at point(1, 1) given a random path: " << prettyLady(8, 8, 4, 4) << endl;
	return 0;
}

template <typename T> void shuffle(vector<T> & v) {
	if (v.size() < 2) return;
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
	unsigned count = 0; // count of # of times the sequence was monotonic.
	for (unsigned i = 0; i < TRIALS; i++) {
		vi.clear();
		for (unsigned j = 0; j < b; j++) {
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
	for (unsigned i = 0; i < b; i++) {
		vi.push_back(rand() % (a - 1));
	}
	unsigned count = 0;
	for (unsigned i = 0; i < TRIALS; i++) {
		vi.clear();
		for (unsigned j = 0; j < b; j++) {
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

double okNesting(unsigned n) {
	unsigned count = 0; // Count the number of positives in our trials.
	// Check the probability of 
	// a sequence of n left parens and n right parens
	// being properly nested
	vector<char> vc; // First build our vector.
	vc.insert(vc.begin(), n, '(');
	vc.insert(vc.begin(), n, ')');
	stack<char> stc; // Stack of chars. Push onto stack if '(', pop if ')'
	for (unsigned i = 0; i < TRIALS; i++) {
		shuffle(vc);
		bool mismatched = 0;
		for (char c : vc) {
			if (c == '(')
				stc.push(c);
			if (c == ')') {
				if (stc.empty()) {  // Mismatched parens.
					mismatched = 1;
					break;
				}
				stc.pop();
			}
 		}
		if (!mismatched)
			count++;
	}
	return double(count) / TRIALS;
}

double duel(double a, double b) {
	unsigned count = 0;
	for (unsigned i = 0; i < TRIALS; i++) {
		bool dead = 0; // Tells us if one of the duellists was unfortunate
		while (!dead) { // Keep going until the duel is over
			dead = outcome(a);
			if (dead) // Mr. A wins
				count++;
			else	
				dead = outcome(b);
		}
	}
	return double(count) / TRIALS;
}

bool outcome(double probability) { // Generates a bool outcome given a probability
	return (rand() / (RAND_MAX + 1.0)) < probability;
}

double flip(double p) {
	return 1;
}

double prettyLady(unsigned gridX, unsigned gridY, unsigned ladyX, unsigned ladyY) {
	vector<char> vc; // This vector will represent paths. 
	// A 0 means a move "right" (x)
	// A 1 means a move "up" (y)
	unsigned count = 0;
	vc.insert(vc.begin(), gridX, 0);
	vc.insert(vc.begin(), gridY, 1);
	char x = 0; // X position
	char y = 0; // Y position
	for (unsigned i = 0; i < TRIALS; i++) {
		shuffle(vc);
		x = 0;
		y = 0;
		for (char c : vc) {
			if (c == 0)
				x++;
			else if (c == 1)
				y++;
			if (x == ladyX && y == ladyY) {
				count++;
				break;
			}
		}
	}
	return double(count) / TRIALS;
}