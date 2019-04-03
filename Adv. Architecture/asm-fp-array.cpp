// CS546 floating Point Lab  Exercise 1
#include <iostream>
#include <iomanip>
#include <float.h>
#define CS546_ARRAY_SIZE 100
using namespace std;

int main(int argc, char* argv[])
{
	unsigned int i, j;
	float q;
	float a = 546.0;
	float b = 1511.0;
	float c = 80386.0;
	float x[CS546_ARRAY_SIZE];
	float results[CS546_ARRAY_SIZE];
	// Fill the array
	for (i = 0; i < CS546_ARRAY_SIZE; i++)
		x[i] = i;
	std::cout << "====================\n";
	__asm {
		// your inline assembler code goes here
		// Quadratic: ax^2 + bx + c
		// Cool, now actually do the math.
		MOV i, 0
		l:
		FLD a
		FIMUL i
		FIMUL i
		FLD b
		FIMUL i
		FADDP st(1), st(0)
		FLD c
		FADDP st(1), st(0)
		MOV eax, i
		FSTP [eax * 4 + results]
		ADD i, 1
		cmp i, 100
		jl l
	}

	for (i = 0; i < CS546_ARRAY_SIZE; i++)
		cout << setw(3) << i << " " << setw(11) << fixed << setprecision(2) << results[i] << endl;
	std::cout << "\nDone." << endl;
	system("pause");
	return 0;
}
