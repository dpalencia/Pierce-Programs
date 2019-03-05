#include <iostream>
#include <set>
using namespace std;
typedef pair<unsigned, unsigned> OP; 
typedef set<pair<unsigned, unsigned>> SOP;
void show(const OP & op);
void show(const SOP & sop);
OP makeOP(unsigned first, unsigned second);
OP add(const OP & a, const OP & b);
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
	system("pause");
	return 0;
}
void show(const OP & op) {
	cout << "OP: ( " << op.first << ", " << op.second << " )";
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