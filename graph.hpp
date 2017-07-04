// -------------------------------------------------------------------
// graph.hpp
// 
// Graph ADT (Abstract Data Type). We model undirected weighted graphs
// using array  (actually an  STL vector) of  edge lists. We  make the
// vertice  value (not  its ID)  and the  edge weight  of customizable
// types, by leveraging  templates.  We use the terms  vertex and node
// interchangeably.

#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <iostream>
#include <cstdlib>
#include <vector>
#include <assert.h>
using namespace std;

// vertID: we  assume that the graph vertices  are uniquely identified
// by an unsigned index.

typedef unsigned vertID;

// Edge: contains  a neighbor  ID and a  generic value of  custom type
// (called  etype). The neighbor  is the  vertex that  has an  edge in
// common  with the  current  vertex.  Val could  be,  for example,  a
// floating-point weight or  a struct containing a string  label and a
// double weight.

template <class etype> 
struct Edge {
  vertID neigh;
  etype val;

  // constructors
  Edge(): neigh(0), val(0) {}
  Edge(vertID neigh, etype val): neigh(neigh), val(val){}

  // operator overload to facilitate viualizing edge
  friend ostream& operator<<
  (ostream& out, Edge<etype>& e) {
    out << "(" << e.neigh << "," << e.val << ")";
    return out;
  }
};

// Vertex/Node: contains  a value of custom  type and a  list of edges
// (implemented as  an STL vector). Both  the value of  the vertex and
// edge must be specified.

template <class vtype, class etype>
class Vertex {
private: 
  // typedefs make the code clearer within this class
  typedef Vertex<vtype,etype> CustomVertex;
  typedef Edge<etype> CustomEdge;

  vtype key; // value stored in the node
  vector<CustomEdge> elist; // list of edges

  // shows the neighbors of this vertex
  void print(ostream& out) {
    for(int i=0; i<elist.size(); ++i) {
      out << elist[i] << ",";
    }
    out << "#";
  }

  // find: utility to find a specific neighbor of this vertex. Returns the
  // index of the found index or -1 if not found.
  int find(vertID neigh) {
    for(int i=0; i<elist.size(); ++i) {
      if(elist[i].neigh == neigh) // lookup by vertID
        return i;
    }
    return -1;
  }

public:
  Vertex(): key(0) {}
  Vertex(vtype key): key(key) {}
  vtype get_key() { return key; }
  void set_key(vtype key) { this->key = key; }

  // add: inserts an edge of weight 'weight' with vertice 'neigh'
  void add(vertID neigh, etype weight) {
    CustomEdge e(neigh, weight);
    elist.push_back(e);
  }

  // is_adjacent: check if this vertex is adjacent to neigh
  bool is_adjacent(vertID neigh) {
    int i = find(neigh);
    return (i >= 0);
  }

  // get_weight: returns the edge weight. Note: the edge MUST exist.
  etype get_weight(vertID neigh) {
    int i = find(neigh);
    assert(i >= 0);
    return elist[i].val;
  }

  // set_weight: modifies the edge weight. Note: the edge MUST exist.
  void set_weight(vertID neigh, etype weight) {
    int i = find(neigh);
    assert(i >= 0);
    elist[i].val = weight;
  }

  // del: removes an edge with the correct vertID from the connections
  // returns true if one edge was removed (false otherwise)
  bool del(vertID neigh) {
    int i = find(neigh);
    if(i >= 0) {
      elist.erase(elist.begin() + i);
      return true; 
    }
    return false;
  }

  // get_neighbors: returns a vector containing the vertIDs of all neighbors of
  // this vector.
  void get_neighbors(vector<vertID>& neigh) {
    neigh.clear();
    for(int i=0; i<elist.size(); ++i)
      neigh.push_back(elist[i].neigh);
  }

  // operator overload to facilitate viualizing 
  friend ostream& operator<<
  (ostream& out, CustomVertex& e) {
    e.print(out);
    return out;
  }

  // destructor: remove all neighbors
  ~Vertex() { elist.clear(); } 
};

template <class vtype, class etype>
class Graph {
private:
  // typedefs make the code clearer within this class
  typedef Vertex<vtype,etype> CustomVertex;
  typedef Edge<etype> CustomEdge;

  unsigned nedges; // total number of edges
  vector<CustomVertex> vlist; // list of vetices (and adjacencies)

  // verify if the vertex index is within the allowed boundary
  void validate_vertex(vertID x) {
    assert(x < get_nodes()); 
  }

  // verifies two vertices simmultaneously
  void validate_vertices(vertID x, vertID y) {
    validate_vertex(x);
    validate_vertex(y);
  }

  // prints the graphs edges organized by vertex
  void print(ostream& out) {
    for(int i=0; i<vlist.size(); ++i)
      for(int j=0; j<i; ++j)
        if(is_adjacent(i,j))
          cout << i << " " << j << " " << get_edge_weight(i,j) << endl;

    //for(int i=0; i<vlist.size(); ++i) {
    //  out << "V[" << i << "]:" << vlist[i] << endl;
    //}
  }

public:
  Graph(): nedges(0){}

  // acessor methods
  unsigned get_nodes() { return vlist.size(); }
  unsigned get_edges() { return nedges; }

  // verifies if there is an edge between x and y
  bool is_adjacent(vertID x, vertID y) {
    validate_vertices(x, y);
    return vlist[x].is_adjacent(y);
  }

  bool is_vertex(vertID x) {
    return (x < get_nodes());
  }

  // returns the weight of the edge between x and y
  etype get_edge_weight(vertID x, vertID y) {
    validate_vertices(x, y);
    return vlist[x].get_weight(y);
  }

  // returns the key of the vertex/node
  vtype get_vertex_key(vertID x) {
    validate_vertex(x);
    return vlist[x].get_key();
  }

  // returns neighbors of vertex v in vector neigh
  void get_neighbors(vertID v, vector<vertID>& neigh) {
    vlist[v].get_neighbors(neigh);
  }

  // mutator methods
  // add a vertex to the graph 
  void add_vertex(vtype key) {
    CustomVertex v(key);
    vlist.push_back(v);
  }

  // adds an edge between vertices x and y and sets their weight
  // if the edge already exists, only update the weight
  void add_edge(vertID x, vertID y, etype weight) {
    validate_vertices(x, y);
    if(!is_adjacent(x,y)) {
      vlist[x].add(y, weight);
      vlist[y].add(x, weight);
      nedges++;
      //cout << "EDGE(" << x << "," << y << ")" << endl;
    } else {
      set_edge_weight(x,y,weight);
    }
  }

  void add_edge(vertID x, vertID y) {
    add_edge(x, y, 1.0); // default weight
  }

  // modifies the weight of the edge between x and y (the edge must exist)
  void set_edge_weight(vertID x, vertID y, etype weight) {
    validate_vertices(x, y);
    vlist[x].set_weight(y, weight);
    vlist[y].set_weight(x, weight);
  }

  // modifies the key of the vertex/node
  void set_vertex_key(vertID x, vtype key) {
    validate_vertex(x);
    vlist[x].set_key(key);
  }

  // operator overload to facilitate viualizing 
  friend ostream& operator<<
  (ostream& out, Graph& g) {
    g.print(out);
    return out;
  }

  // deallocate al vertices and their respective adjacency lists
  void clear() { vlist.clear(); }

  // creates a copy of g into *this
  void clone(Graph<vtype,etype>& g) {
    // copy all vertices
    for(vertID i=0; i<g.get_nodes(); ++i) {
      add_vertex(g.get_vertex_key(i));
    }

    // copy all edges: assumes that the graph is undirected and without loops
    for(vertID i=0; i<g.get_nodes(); ++i) {
      for(vertID j=0; j<i; ++j) {
        if(g.is_adjacent(i,j)) {
          add_edge(i, j, g.get_edge_weight(i,j));
        }
      }
    }
  }

  // clears the graph for reuse
  ~Graph() { clear(); }
};

#if 0
// -------------------------------------------------------------------
// testing functions

// mkrand creates a random number of double format using an integer part and a
// fractional part with up to 2 decimal digits. I won't call srand inside
// mkrand to give the user the freedom to do that outside of this function.

double mkrand(double min, double max) { 
  unsigned x = static_cast<unsigned>( 
    (rand() % (static_cast<int>((max-min)*100.0)))
  );

  return min + (static_cast<double>(x)/100.0);
}

void test_vertex() {
  Vertex<int,double> v(666);
  srand(time(0));
  for(vertID i=0; i<30; ++i) 
    v.add(i, mkrand(1.0,10.0));

  cout << "v={" << v << "}" << endl << endl;

  for(vertID i=0; i<30; i+=2) 
    v.del(i);

  cout << "v={" << v << "}" << endl << endl;

  cout << "v adjacent to " << 5 << "? " << v.is_adjacent(5) << endl;
  cout << "v adjacent to " << 6 << "? " << v.is_adjacent(6) << endl;

  for(vertID i=0; i<30; ++i) 
    v.del(i);

  cout << "v={" << v << "}" << endl << endl;
}

void create_graph (int size, double density, double min, double max) {
  Graph<vertID,double> g;

  // insert vertices
  for(vertID i=0; i<size; ++i) 
    g.add_vertex(i);

  // for all possible edges (except loops), insert edges with prob. density
  for(vertID i=0; i<size; ++i) {
    for(vertID j=0; j<size; ++j) {
      if(i == j) continue; // skip loops

      if(mkrand(0.0,1.0) < density)
        g.add_edge(i, j, mkrand(min,max)); // random weight
    }
  }

  cout << g << endl;
  cout << endl;
  g.set_vertex_key(5,777);
  g.set_edge_weight(5,9,9.99);

  cout << g << endl;

  cout << "Vertex 5 has key " << g.get_vertex_key(5) << endl;

  vector<vertID> neigh;
  for(vertID i=0; i<g.get_nodes(); ++i) {
    g.get_neighbors(i,neigh);
    cout << "Neighbors of " << i << ": ";
    for(vertID j=0; j<neigh.size(); ++j) 
      cout << neigh[j] << ",";
    cout << "null" << endl;
  }
  neigh.clear();
}

void test_edge() {
  Edge<double> e;
  e.val = 666.0;
  cout << e.val << " => " << e << endl;
}

int main() {
  //test_edge();
  //test_vertex();
  create_graph(10, 0.2, 1.0, 10.0);
}
#endif
#endif
