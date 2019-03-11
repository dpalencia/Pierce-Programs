// Palencia, Daniel CS 546 Section 13495 2/27/19
// First Laboratory Assignment - Compute Pythagorean Triples
#include <iostream>
using namespace std;
int main() {
	int m, n, a, b, c;
	int flags = 0; // 4 byte int that we will pop flags onto if we get an error
	cout << "Pythagorean Triples Calculation - CS 546 - Daniel Palencia" << endl;
	cout << "Enter values for m and n:" << endl;
	cin >> m >> n;
	while (m <= 0 || n <= 0) {
		if (m <= 0)
			cout << "Value " << m << " is out of range - only positive integers are allowed" << endl;
		if (n <= 0)
			cout << "Value " << n << " is out of range - only positive integers are allowed" << endl;
		cin >> m >> n;
	}
	__asm {
		mov eax, m
		imul eax, n
		jo f // Overflow check each time we multiply
		imul eax, 2
		jo f
		mov a, eax
		mov eax, m
		imul eax, m
		jo f
		mov ebx, n
		imul ebx, n
		jo f
		sub ebx, eax
		js f // Check if the subtraction resulted in a negative
		jz f // Check if the subtraction resulted in a 0
		mov b, ebx
		mov eax, m
		imul eax, m
		jo f
		mov ebx, n
		imul ebx, n
		jo f
		add eax, ebx
		jo f
		mov c, eax
	}

	cout << "Side a = " << a << endl;
	cout << "Side b = " << b << endl;
	cout << "Side c = " << c << endl;
	cout << "Calculation ended." << endl;
	system("pause");
	return 0; // Returning 0 if calculations completed

	__asm {
	f:
		pushfd // Copy EFLAGS register onto stack and pop it into flags variable
		pop flags 
	}
	cout << "Error completing calculation - Flags:" << hex << flags << endl;
	system("pause");
	return -1; // Returning -1 if not
}




