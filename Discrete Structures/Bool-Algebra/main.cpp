#include <iostream>
#include <stack>
#include <string>
#include <sstream>
using namespace std; 
typedef stack<bool> oprnd;
typedef stack<char> oprtr;
int prec(char c); // Check precedence of a char represented as an int
void evaluateTop(oprnd & oprnd, oprtr & oprtr); // Evaluate until it hits ( or end of input
int main() {
	oprnd oprnd;
	oprtr oprtr;
	string input;
	char c;
	getline(cin, input);
	istringstream line(input); 
	oprnd.push(line.get());
	oprtr.push(line.get());
	// implement whitepsace ignore
	while (line.get(c)) {
		if (c == '(')
			oprtr.push(c);
		else if (c == ')') {
			while (oprtr.top() != '(') {
				evaluateTop(oprnd, oprtr);
			}
			oprtr.pop(); // discard that left paren
		}
		while (prec(c) <= prec(oprnd.top())) {
			evaluateTop(oprnd, oprtr);
		}
	}	
	evaluateTop(oprnd, oprtr);
	cout << oprnd.top() << endl;
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
			oprnd.push(l | r);
			oprnd.pop();
			return;
	}
	return;
}

int prec(char c) {
	switch (c) {
		case '!':
			return 3;
		case '&':
			return 2;
		case '^':
			return 1;
		case '|':
			return 0;
	}
}
