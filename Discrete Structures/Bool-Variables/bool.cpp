// Palencia, Daniel
// 4-10-19
// CS 542 Discrete Structures
// Homework #10 Bool Equivalency
#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <iterator>
using namespace std;
typedef stack<bool> oprnd;
typedef stack<char> oprtr;
int prec(char c); // Check precedence of a char represented as an int
void evaluateTop(oprnd & oprnd, oprtr & oprtr); // Evaluate until it hits ( or end of input
bool isletter(char c); // Check if c is a letter. Letters are our "variables"
bool evaluate(const string & exp); // Evaluates a string expression and returns the answer
string vars(const string & exp); // Fills up a set with our variables and returns it
void split(const string & inputstring, string & banana, string & split); // Take an input string, split it into two at the comma
bool inc(string & bools); // Increments our string representing a binary number, returns false if no increment happened
int main() {
	// Build a string of 0's, same length as variable string
	// Loop through, replacing each variable with the corresponding bool value, then increment the bits
	// i.e. 00, 01, 10, 11 or 000, 001, 010, 011, 111
	string input;
	string workexpr;
	string exp1; // Expression 1 
	string exp2; // Expression 2
	string bools; // Holds a string containing all of our bools to increment
	string varstr; // A set containing all of our variables
	bool equiv; // Tells us if an expression is equivalent
	cout << "Enter two bool expressions with letter variables (a-z), (A-Z), to see if they are equivalent.\n";
	for (getline(cin, input); input != ""; getline(cin, input)) {
		equiv = 1; // Set to 1 to begin
		// Check if our character is an operator and get each variable
		varstr = vars(input); // Get our set of variables
		// Build a string of 0's, same length as variable set
		bools = string(varstr.size(), '0');
		// Loop through each permutation of values with each variable getting replaced by its corresponding bool value
		do {
			// Replace the variables with the appropriate values
			workexpr = string(input); // Copy input into our expression
			for (int i = 0; i < input.length(); i++) {
				if (varstr.find(input[i]) != string::npos) {
					workexpr[i] = bools[varstr.find(input[i])];
				}
			}
			// Split the input string into two, delimited by a comma
			split(workexpr, exp1, exp2);
			if (evaluate(exp1) != evaluate(exp2)) {
				equiv = 0;
			}
		} while (inc(bools) && equiv == 1);
		equiv ? cout << "Equivalent.\n" : cout << "Not equivalent.\n";
	}
	system("pause");
	return 0;
}

void evaluateTop(oprnd & oprnd, oprtr & oprtr) {
	bool r = oprnd.top(); oprnd.pop();
	bool l;
	char c = oprtr.top(); oprtr.pop();
	switch (c) {
	case '!':
		oprnd.push(!r);
		return;
	case '&':
		l = oprnd.top();
		oprnd.pop();
		oprnd.push(l & r);
		return;
	case '^':
		l = oprnd.top();
		oprnd.pop();
		oprnd.push(l ^ r);
		return;
	case '|':
		l = oprnd.top();
		oprnd.pop();
		oprnd.push(l | r);
		return;
	}
	return;
}

int prec(char c) {
	switch (c) {
	case '!':
		return 5;
	case '&':
		return 4;
	case '^':
		return 3;
	case '|':
		return 2;
	case '(':
		return 1;
	}
}

bool isletter(char c) { // Helper function that checks if a char is an operator. If not, it is a variable.
	if ((c >= 97 && c <= 122) || (c >= 65 && c <= 90)) // If the ASCII value of 'c' is a letter (A-Z) || (a-z)
		return 1;
	return 0;
}


bool evaluate(const string & exp) { // Evaluates a string expression and returns the answer
	char c;
	oprnd oprnd; // Stack of bools
	oprtr oprtr; // Stack of chars
	istringstream line(exp);
	line >> c;
	while (!line.eof()) {
		if ((c == 48 || c == 49)) // Push if it's a bool value (1 or 0)
			oprnd.push(c - 48);
		else { // Otherwise it's an operator
			if (c == ')') { // Evaluate until the left paren
				while (oprtr.top() != '(')
					evaluateTop(oprnd, oprtr);
				oprtr.pop(); // discard the left paren
			}
			else if (oprtr.empty() || c == '(' || prec(c) > prec(oprtr.top())) {
				oprtr.push(c);
			}
			else if (prec(oprtr.top()) >= prec(c)) { // if the precedence of the preceding operator is >=, evaluate from stack
				while (!oprtr.empty() && oprtr.top() != '(' && prec(oprtr.top()) >= prec(c)) { // keep going until stack top precedence is not >=
					evaluateTop(oprnd, oprtr);
				}
				oprtr.push(c); // then push the operator onto the stack and continue
			}
		}
		line >> c;
	}
	while (!oprtr.empty())
		evaluateTop(oprnd, oprtr); // keep evaluating until operator stack is empty
	cout << exp << " = " << oprnd.top() << endl;
	return oprnd.top();
}


string vars(const string & exp) { // Fills up a string with variables and returns it
	string ret;
	for (char c : exp) {
		if (isletter(c) && ret.find(c) == string::npos) { // If c is a letter, it's a variable. Push into our vector (if not already in our v
			ret.push_back(c); // Add to our string
		}
	}
	return ret;
}

void split(const string & inputstring, string & banana, string & split) { // Split our string at the comma
	size_t comma = inputstring.find(',');
	banana = inputstring.substr(0, comma);
	split = inputstring.substr(comma + 1, string::npos);
}

bool inc(string & bools) {
	for(int i = bools.length() - 1; i >= 0; i--) { // Loop "backwards" through the string
		cout << bools << endl;
		if (bools[i] == '0') {
			bools[i]++;
			for (int j = i + 1; j < bools.length(); j++) 
				bools[j] = '0';
			return 1;
		}
	}
	return 0;
}