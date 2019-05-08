#include <iostream>
#include <map>
#include <set>
#include <queue>
using namespace std;
typedef int Vertex; // For our purposes a vertex is an int
typedef set<Vertex> VertexSet; // 
typedef map<Vertex, VertexSet> Graph; // Adjacency list representation
void bfs(const Graph& g2, Vertex start);
class Node {
	int d;
	Vertex v;
};
int main() {
	Graph g;
	return 0;
}
typedef vector<Node> vn;
void bfs(const Graph& g2, Vertex start) {
	queue<Node> q; // The queue
	vn vn; // Vector of nodes
	for (auto it = g2.begin(); it != g2.end(); it++) {
		
	}
	// G2 is a valid graph.
	// Start is a vertex of g2.
	// Output all vertices connected to the start.
	// Output their distances to start
	// Possible that g2 is not a connected graph.
	// Vertices visited in breadth-first order,
	// With the neighbors of a vertex visited
	// in increasing order.

	// Algorithm:
		// Set every node's distance to -1.
		// Start node -> distance = 0
		// While (queue is not empty)
			// find smallest numbered neighbor
			// set n's distance to f's distance + 1
}


