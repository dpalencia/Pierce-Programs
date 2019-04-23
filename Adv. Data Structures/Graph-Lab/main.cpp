#include <iostream>
#include <set>
#include <utility>
#include <cassert>
using namespace std;
typedef int Vertex; // Each vertex is an int
typedef set<Vertex> VertexSet; // Set of vertices
typedef set<Vertex> Edge; // An edge is a pair, 2 vertices. Undirected because it's a set
typedef set<Edge> EdgeSet; // Set of edges (set of sets of 2 vertices)
typedef pair<VertexSet, EdgeSet> Graph; // Graph is represented as a set of vertices + a set of edges | Graph = (V, E)
void graphShow(const Graph & g);
Graph createComplete(unsigned v); // Creates a complete graph with v verts
bool isComplete(const Graph & g); // Checks if a graph is complete
Graph createCycle(unsigned v); // Create a cycle graph
bool isCycle(const Graph & g); // Return if it's a cycle graph
Graph createWheel(unsigned v); // Create a wheel graph
bool isWheel(const Graph & g); // Return if it's a wheel graph
int main() {
	Graph complete = createComplete(5);
	Graph cycle = createCycle(5);
	Graph wheel = createWheel(5);
	for (int i = 0; i < 2; i++) {
		if (isComplete(complete)) {
			cout << "This graph is complete: " << endl;
			graphShow(complete);
		}
		else if (!isComplete(complete)) {
			cout << "This graph is not complete: " << endl;
			graphShow(complete);
		}
		if (isCycle(cycle)) {
			cout << "This graph is a cycle: " << endl;
			graphShow(cycle);
		}
		else if (!isCycle(cycle)) {
			cout << "This graph is not a cycle: " << endl;
			graphShow(cycle);
		}
		if (isWheel(wheel)) {
			cout << "This graph is a wheel: " << endl;
			graphShow(wheel);
		}
		else if (!isWheel(wheel)) {
			cout << "This graph is not a wheel: " << endl;
			graphShow(wheel);
		}
		cycle.second.erase(cycle.second.begin());
		complete.second.erase(complete.second.begin());
		wheel.second.erase(--wheel.second.end());
	}
	
	
	system("pause");
	return 0;
}


void graphShow(const Graph & g) {
	cout << "Vertices:\n";
	for (Vertex v : g.first)
		cout << v << " ";
	cout << "\nEdges:\n";
	for (Edge e : g.second) {
		auto it = e.begin();
		cout << "(" << *it << ", ";
		cout << *(++it) << ")" << endl;
	}
}


Graph createComplete(unsigned v) {
	Graph ret; // Our return graph
	for (unsigned i = 0; i < v; i++) 
		ret.first.insert(i); // Initialize our vertices
	
	for (Vertex v : ret.first)  // For each vertex,
		for (Vertex w : ret.first) // Look at each other vertex,
			if (w != v) // Make sure it's not the same vertex,
				ret.second.insert(Edge({v, w})); // And insert an edge with the two vertices.
	// Because each edge in edgeset is a set, we don't need to worry about duplicates.
	return ret;
}

bool isComplete(const Graph & g) {
	for (Vertex v : g.first) // For each vertex,
		for (Vertex w : g.first) // Look at each other vertex,
			if (w != v) // Make sure it's not the same vertex,
				if (g.second.count(Edge({ v, w })) == 0)  // Check if there is an edge with the two verts
					return 0; // If not, it is not a complete graph.
	return 1;
}

Graph createCycle(unsigned v) {
	Graph ret;
	for (int i = 0; i < v; i++) // Initialize the graph with each vertex
		ret.first.insert(i);
	for (Vertex v : ret.first) { // For each vertex except the last vertex
		if (ret.first.upper_bound(v) != ret.first.end())
			ret.second.insert(Edge{ v, *(ret.first.upper_bound(v)) }); // Create an edge with it and the next vertex
	}
	// Create the edge with last and first verts to complete the cycle
	ret.second.insert(Edge{ *(ret.first.begin()), *(--ret.first.end()) });
	return ret;
}

bool isCycle(const Graph & g) {
	for (Vertex v : g.first)  // For each vertex except the last vertex
		if (v != *(--g.first.end()) && g.second.count(Edge({v, *(g.first.upper_bound(v))})) == 0) // Check if there is an edge with it and the next vertex
			return 0;
	if (g.second.count(Edge({ *(g.first.begin()), *(--g.first.end()) }))) // Check if first and last verts are connected
	// 	return 0;
	return 1;
}

Graph createWheel(unsigned v) { // Create a wheel graph
	// First create a cycle
	Graph ret = createCycle(v - 1);
	for (Vertex vert : ret.first) { // Connect each edge in the cycle to our new vert
		ret.second.insert(Edge{ vert, (Vertex)(v - 1) });
	}
	// Insert the new vertex
	ret.first.insert(v - 1);
	return ret;
}

bool isWheel(const Graph & g) {
	// Every wheel contains a cycle
	if (!isCycle(g))
		return 0;
	// Here we know it's a cycle: check if there exists a vertex connected once to each other vertex
	// The vertex with >2 edges is the "center" of the wheel; check for this
	for (Vertex v : g.first) { // For each vertex
		int edgeCount = 0; // This counts how many edges the vertex appears in
		for (Edge e : g.second) { // Check each edge
			if (e.count(v)) { // If the vertex exists in an edge, increment
				if (++edgeCount > 2) // The check (vertex with >2 edges is the center of the wheel)
					return 1;
			}
		}
	}
	// If we make it this far we know there is no "center" vertex
	return 0;
}
/* 

/*
v >= 4
The first func's job is to create and return a wheel graph with v vertices.
The second func's job is to return whether g is a wheel graph */