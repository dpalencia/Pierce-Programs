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
				while (!oprtr.empty() && oprtr.top() != '(' && prec(oprtr.top() >= prec(c))) { // keep going until stack top precedence is not >=
					evaluateTop(oprnd, oprtr);
				}
				oprtr.push(c); // then push the operator onto the stack and continue
			}
		}
		line >> c;
	}
 	while(!oprtr.empty())
		evaluateTop(oprnd, oprtr); // keep evaluating until operator stack is empty
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
			oprnd.pop();
			oprnd.push(l | r);
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
		case '(':
			return -1;
	}
}
