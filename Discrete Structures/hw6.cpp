#include <iostream> 
#include <set>
#include <cassert>
using namespace std;
typedef pair<unsigned, unsigned> OP;
typedef set<OP> SOP;
bool er(const SOP & sop, const set<unsigned> & univ);
bool reflexive(const SOP & sop, const set<unsigned> & univ);
bool symmetric(const SOP & sop);
bool transitive(const SOP & sop);
int main() {
	SOP s = { {1, 2}, {1, 3}, {1, 1}, {2, 1}, {2, 3}, {3, 2}, {3, 1}, {1, 1}, {2, 2}, {3, 3} };
	SOP p = { {1, 2}, {2, 3} };
	set<unsigned> u = { 1, 2, 3 };
	assert(er(s, u));
	cout << er(s, u) << endl;
	system("pause");
}
bool er(const SOP & sop, const set<unsigned> & univ) {
	return reflexive(sop, univ) && symmetric(sop) && transitive(sop);
}
bool reflexive(const SOP & sop, const set<unsigned> & univ) {
	for (unsigned i : univ) {
		if (!sop.count({ i, i })) {
			return false;
		}
	}
	return true;
}
bool symmetric(const SOP & sop) {
	for (auto it = sop.begin(); it != sop.end(); it++) {
		if (!(sop.count({ (*it).first, (*it).second }) && sop.count({ (*it).second, (*it).first }))) {
			return false;
		}
	}
	return true;
}
bool transitive(const SOP & sop) {
	unsigned a, b;
	for (auto it = sop.begin(); it != sop.end(); it++) {
		a = (*it).first;
		b = (*it).second;
		for (auto it = sop.begin(); it != sop.end(); it++) {
			if ((*it).first == b) {
				if (!sop.count({ a, (*it).second }))
					return false;
			}
		}
	}
	return true;
}
