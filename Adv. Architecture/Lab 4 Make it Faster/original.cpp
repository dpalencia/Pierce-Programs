// The following program was extracted from:
// https://www.quantstart.com/articles/European-vanilla-option-pricing-with-C-via-Monte-Carlo-methods
// It has been modified for use as a CS546 lab assignment.

#include <algorithm>    // Needed for the "max" function
#include <cmath>
#include <iostream>
#include <climits> // added for unsigned max
#include <Windows.h> // added for timing functions, etc
ULONGLONG getTime64(LPFILETIME a); // added
using namespace std; // added

// A simple implementation of the Box-Muller algorithm, used to generate
// gaussian random numbers - necessary for the Monte Carlo method below
// Note that C++11 actually provides std::normal_distribution<> in 
// the  library, which can be used instead of this function
double gaussian_box_muller() {
	double x = 0.0;
	double y = 0.0;
	double euclid_sq = 0.0;

	// Continue generating two uniform random variables
	// until the square of their "euclidean distance" 
	// is less than unity
	do {
		x = 2.0 * rand() / static_cast(RAND_MAX)-1;
		y = 2.0 * rand() / static_cast(RAND_MAX)-1;
		euclid_sq = x * x + y * y;
	} while (euclid_sq >= 1.0);

	return x * sqrt(-2 * log(euclid_sq) / euclid_sq);
}

// Pricing a European vanilla call option with a Monte Carlo method
double monte_carlo_call_price(const int& num_sims, const double& S, const double& K, const double& r, const double& v, const double& T) {
	double S_adjust = S * exp(T * (r - 0.5 * v * v));
	double S_cur = 0.0;
	double payoff_sum = 0.0;

	for (int i = 0; i < num_sims; i++) {
		double gauss_bm = gaussian_box_muller();
		S_cur = S_adjust * exp(sqrt(v * v * T) * gauss_bm);
		payoff_sum += max(S_cur - K, 0.0);
	}

	return (payoff_sum / static_cast(num_sims)) * exp(-r * T);
}

// Pricing a European vanilla put option with a Monte Carlo method
double monte_carlo_put_price(const int& num_sims, const double& S, const double& K, const double& r, const double& v, const double& T) {
	double S_adjust = S * exp(T * (r - 0.5 * v * v));
	double S_cur = 0.0;
	double payoff_sum = 0.0;

	for (int i = 0; i < num_sims; i++) {
		double gauss_bm = gaussian_box_muller();
		S_cur = S_adjust * exp(sqrt(v * v * T) * gauss_bm);
		payoff_sum += max(K - S_cur, 0.0);
	}

	return (payoff_sum / static_cast(num_sims)) * exp(-r * T);
}

int main(int argc, char** argv) {
	// First we create the parameter list                                                                               
	int num_sims = INT_MAX; // was 10000000;   // Number of simulated asset paths                                                       
	double S = 100.0;  // Option price                                                                                  
	double K = 100.0;  // Strike price                                                                                  
	double r = 0.05;   // Risk-free rate (5%)                                                                           
	double v = 0.2;    // Volatility of the underlying (20%)                                                            
	double T = 1.0;    // One year until expiry                                                                         

	// Timing code added before and after computationally intensive code segment
	FILETIME creationTime, exitTime, kernelTime, userTime;
	LPFILETIME creation = &creationTime, exit = &exitTime, kernel = &kernelTime, user = &userTime;
	ULONGLONG c1, c2, e1, e2, k1, k2, u1, u2;
	HANDLE myProcess = GetCurrentProcess();
	BOOL gotTime1, gotTime2;
	DWORD failReason;
	SYSTEMTIME loct;
	cout << "Enter number of simulations to run:";
	cin >> num_sims;
	cout << endl;

	gotTime1 = GetProcessTimes(myProcess, creation, exit, kernel, user);
	if (!gotTime1) {
		failReason = GetLastError();
		cout << "GetProcessTimes failed, Failure reason:" << failReason << endl;
		return 0;
	}
	c1 = getTime64(creation);
	e1 = getTime64(exit);
	k1 = getTime64(kernel);
	u1 = getTime64(user);

	// do something long...
	GetLocalTime(&loct);
	cout << "The starting local time is:" << loct.wHour << ':' << loct.wMinute << ':' << loct.wSecond << '.' << loct.wMilliseconds << endl;
	double fStartTime = loct.wHour * 3600 + loct.wMinute * 60 + loct.wSecond + (loct.wMilliseconds / 1000.0);

	// Then we calculate the call/put values via Monte Carlo                                                                          
	double call = monte_carlo_call_price(num_sims, S, K, r, v, T);
	double put = monte_carlo_put_price(num_sims, S, K, r, v, T);

	// Add code to display CPU time
	gotTime2 = GetProcessTimes(myProcess, creation, exit, kernel, user);
	if (!gotTime2) {
		failReason = GetLastError();
		cout << "GetProcessTimes failed, Failure reason:" << failReason << endl;
		return 0;
	}
	GetLocalTime(&loct);
	cout << "The   ending local time is:" << loct.wHour << ':' << loct.wMinute << ':' << loct.wSecond << '.' << loct.wMilliseconds << endl;
	double fEndTime = loct.wHour * 3600 + loct.wMinute * 60 + loct.wSecond + (loct.wMilliseconds / 1000.0);
	u2 = getTime64(user);
	double workUserTime = (u2 - u1) * 100.0 / 1000000000.0;
	cout << "Process user time:" << workUserTime << " (" << u2 - u1 << ')' << endl;

	double workTime = fEndTime - fStartTime;
	cout << "Elapsed clock time:" << workTime << endl;
	cout << "CPU busy percentage:" << (workUserTime / workTime) * 100 << endl;

	// Finally we output the parameters and prices                                                                      
	std::cout << "Number of Paths: " << num_sims << std::endl;
	std::cout << "Underlying:      " << S << std::endl;
	std::cout << "Strike:          " << K << std::endl;
	std::cout << "Risk-Free Rate:  " << r << std::endl;
	std::cout << "Volatility:      " << v << std::endl;
	std::cout << "Maturity:        " << T << std::endl;
	std::cout << "Call Price:      " << call << std::endl;
	std::cout << "Put Price:       " << put << std::endl;

	return 0;
}

// convert a FILETIME structure to a 64-bit integer
ULONGLONG getTime64(LPFILETIME a) {
	ULARGE_INTEGER work;
	work.HighPart = a->dwHighDateTime;
	work.LowPart = a->dwLowDateTime;
	return work.QuadPart;
}