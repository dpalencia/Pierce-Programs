#include <iostream>
#include <set>
#include <map>
#include <stack>
using namespace std;
typedef int Vertex;
typedef set<Vertex> VertexSet;
typedef map<Vertex, VertexSet> Graph2;
Graph2 cycle(unsigned n);
void show(const Graph2& g2);
Graph2 cycle(unsigned n);
Graph2 wheel(unsigned n);
Graph2 complete(unsigned n);
Graph2 cb(unsigned a, unsigned b);
bool isCycle(const Graph2& g2);
bool isWheel(const Graph2& g2);
bool isComplete(const Graph2& g2);
int main() {
	Graph2 cyc, wh, comp, compb;
	cyc = cycle(5); cout << "Cycle "; show(cyc);
	wh = wheel(5); cout << "Wheel "; show(wh);
	comp = complete(5); cout << "Complete ";  show(comp);
	compb = cb(5, 5); cout << "CB "; show(compb);
	isCycle(cyc);
	isCycle(wh);
	isWheel(cyc);
	isWheel(wh);
	isComplete(wh);
	isComplete(comp);
	return 0;
}

bool isComplete(const Graph2& g2) {
	for (auto it = g2.begin(); it != g2.end(); it++) {
		if (it->second.size() != (g2.size() - 1)) return false;
	}
	return true;
}

bool isCycle(const Graph2& g2) {
	map<Vertex, bool> marks; // DFS marks
	stack<Vertex> st; // DFS stack
	Vertex top = g2.begin()->first;
	st.push(g2.begin()->first);
	marks[top] = 1;
	while (!st.empty()) {
		top = st.top(); st.pop();
		for (Vertex v : g2.at(top)) {
			if (!marks[v]) {
				if (g2.at(v).size() != 2) return false;
				marks[v] = 1;
				st.push(v);
			}
		}
	}
	for (auto it = marks.begin(); it != marks.end(); it++) {
		if (it->second == 0) return false;
	}
	if (marks.size() == g2.size()) return true;
}

bool isWheel(const Graph2& g2) {
	Vertex center = -1;
	for (auto it = g2.begin(); it != g2.end(); it++) {
		if (it->second.size() == (g2.size() - 1)) {
			center = it->first;
			break;
		}
	}
	if (center == -1) return false;
	map<Vertex, bool> marks; // DFS marks
	stack<Vertex> st; // DFS stack
	st.push(center);
	marks[center] = 1;
	while (!st.empty()) {
		Vertex top;
		top = st.top(); st.pop();
		for (Vertex v : g2.at(top)) {
			if (!marks[v]) {
				if (g2.at(v).size() != 3) return false;
				marks[v] = 1;
				st.push(v);
			}
		}
	}
	for (auto it = marks.begin(); it != marks.end(); it++) {
		if (it->second == 0) return false;
	}
	if (marks.size() == g2.size()) return true;
}

void show(const Graph2& g2) {
	cout << "-------------" << endl;
	for (auto it = g2.begin(); it != g2.end(); it++) {
		cout << it->first << ": [ ";
		for (Vertex v : it->second) {
			cout << v << " ";
		}
		cout << "]" << endl;
	}

}

void connect(Graph2 & g2, unsigned i, unsigned j) {
	g2[i].insert(j);
	g2[j].insert(i);
}

Graph2 cycle(unsigned n) {
	Graph2 ret;
	for (unsigned i = 0; i < n - 1; i++) {
		connect(ret, i, i + 1);
	}
	connect(ret, n - 1, 0);
	return ret;
}

Graph2 wheel(unsigned n) {
	Graph2 ret = cycle(n - 1);
	for (auto it = ret.begin(); it != --ret.end(); it++) {
		connect(ret, it->first, n - 1);
	}
	return ret;
} 

Graph2 complete(unsigned n) {
	Graph2 ret;
	for (unsigned i = 0; i < n; i++) {
		for (unsigned j = 0; j < n; j++) {
			if (i != j) connect(ret, i, j);
		}
	}
	return ret;
}

Graph2 cb(unsigned a, unsigned b) {
	Graph2 ret;
	unsigned size = a + b;
	for (unsigned i = 0; i < a; i++) {
		for (unsigned j = a; j < size; j++) {
			connect(ret, i, j);
		}
	}
	return ret;
}


/*

Heap (where location 0 is unused)
	Is this array a heap?
Func returns whether a given Graph2 is
	Cycle
	Wheel
	Complete

	
	*/