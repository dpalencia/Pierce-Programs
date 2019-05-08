// Palencia, Daniel
// 5-8-19
// CS 532 Advanced Data Structures
// Homework #13 Enumerate nodes & distances with BFS and DFS
#include <iostream>
#include <map>
#include <set>
#include <queue>
#include <stack>
using namespace std;
typedef int Vertex;
typedef set<Vertex> VertexSet; 
typedef map<Vertex, VertexSet> Graph; // Adjacency list representation
void bfs(const Graph& g2, Vertex start);
void dfs(const Graph& g2, Vertex start);
typedef map<Vertex, int> mvi; // Parallel map that gives us distance for each vert, or marks if we're in DFS.
void show(const Graph& g);
int main() {
	Graph g;
	g[1] = { 3,4,5,6 };
	g[2] = { 3,4 };
	g[3] = { 1,2,5 };
	g[4] = { 1, 2 };
	g[5] = { 1, 3, 6 };
	g[6] = { 1, 5 };
	g[7] = { 8, 9 };
	g[8] = { 7, 9 };
	g[9] = { 7, 8 };
	cout << "The graph we're looking at: " << endl;
	show(g);
	cout << "Breadth first search: " << endl;
	bfs(g, 3);
	cout << "Depth first search: " << endl;
	dfs(g, 3);
	Graph g2;
	g2[1] = { 2, 3, 4 };
	g2[2] = { 1, 3 };
	g2[3] = { 1, 2 };
	g2[4] = { 1 };
	g2[5] = { 6 };
	g2[6] = { 5 };
	cout << "The graph we're looking at " << endl;
	show(g2);
	cout << "Depth first search: " << endl;
	bfs(g2, 3);
	cout << "Breadth first search: " << endl;
	dfs(g2, 3);
	system("pause");
	return 0;
}
void bfs(const Graph & g2, Vertex start) {
	mvi mvi;
	for (auto it = g2.begin(); it != g2.end(); it++) {
		mvi[it->first] = -1;
	}
	queue<Vertex> q;
	q.push(start); // Load in the start vert.
	mvi[start] = 0; // Initial distance is 0.
	cout << start << ": " << mvi[start] << endl; // Show the intial vert and its distnace.
	Vertex f;
	while (q.size() != 0) { // Keep processing until the queue is empty.
		f = q.front(); q.pop();
		for (Vertex v : g2.at(f)) { // Look at everything the front vert is adjacent to.
			if (mvi[v] == -1) { // If it hasn't been seen yet, push it into q.
				mvi[v] = mvi[f] + 1; // The distance is front + 1.
				cout << v << ": " << mvi[v] << endl;
				q.push(v);
			}
		}
	}	
}

void dfs(const Graph& g2, Vertex start) {
	// Initialize parallel vertex map to -1. -1 = unmarked, 0 = marked.
	mvi mvi;
	for (auto it = g2.begin(); it != g2.end(); it++) 
		mvi[it->first] = -1;
	// Initialize stack. Push start vert onto. Mark start vert and output.
	stack<Vertex> st;
	st.push(start);
	mvi[start] = 0; 
	cout << start << endl;
	bool noAdjacent;
	VertexSet vs = g2.at(st.top());
	// The algorithm. Examine top of stack for smallest adjacent vertex and push it,
	// or pop if there are no adjacent verts.
	while (!st.empty()) {
		noAdjacent = 1; // Keep track of whether we push to stack or not.
 		vs = g2.at(st.top()); // Copy adjacent vertex set so we can work with it. 
		for (auto it = vs.begin(); it != vs.end() && noAdjacent == 1; it++) { 
			// Exit loop on first (and smallest) adjacent vert we see.
			if (mvi[*it] == -1) { // When we see an unmarked adjacent vert: push output, mark, and exit loop.
				cout << *it << endl;
				st.push(*it);
				mvi[*it] = 0;
				noAdjacent = 0; // Indicate if we saw an adjacent vert.
			}
		}
		if (noAdjacent)
			st.pop();
	}
}

void show(const Graph& g) { // Show helper function
	for (auto it = g.begin(); it != g.end(); it++) {
		cout << it->first << ": { ";
		for (Vertex v : it->second) {
			cout << v << " ";
		}
		cout << "} " << endl;
	}
}


