// **************************************************************************
//  File       [CycleBreaking.h]
//  Author     [Samuel Liu]
//  Synopsis   [the header file for CycleBreaking]
//  Modify     [2022/01/11 Samuel Liu]
// **************************************************************************

#ifndef _CYCLEBREAKING_H
#define _CYCLEBREAKING_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <vector>
using namespace std;

typedef pair<int, int> AdjacentPairs;

// define each Vertex Color as Number
#define WHITE 0
#define GRAY  1
#define BLACK 2

struct Subset
{
    int parent; //vector parent
    int rank; // vector rank
};

struct Edge
{
  int u; // edge start point
  int v; // edge end point
  int weight; // edge weight

  bool operator<(const Edge &e) const {
    return (this->weight < e.weight);
  }
  bool operator==(const Edge &e) const {
    return (e.u == u) && (e.v == v) && (e.weight == weight);
  }
  Edge &operator=(const Edge &e) {
    u = e.u;
    v = e.v;
    weight = e.weight;
    return *(this);
  }

};

class CreateGraph
{
    public:
        CreateGraph(int, int, bool);
        ~CreateGraph();
        void DFS(const AdjacentPairs &, int, int *, bool *, vector<AdjacentPairs> &, vector<Edge*> &, bool *);
        void MFASPbyDFS(vector<Edge*> &);
        void KruskalMST(vector<Edge*> &);
        void AddEdge(int &, int &, int &);
        bool HaveCycle() const;
        bool HaveCycleDFS(int &, int *) const;
        void DirFindCycle(vector<Edge*> &);
        void DirRemoveEdgeE(int &, int &, int &, bool *, vector<AdjacentPairs> &, vector<Edge*> &, bool *);

    private:
        int  NV = 0;
        int  NE = 0;
        bool DIR = false;
        // create adjacency list of vertices
        list<AdjacentPairs> *ListOfAdjacentPairs;
        vector<Edge> VectorOfEdge;
};

class DisjoinSet
{
    public:
        DisjoinSet(int &);
        ~DisjoinSet();
        void MakeSet();
        int  FindSet(int &) const;
        void Union(int &, int &);
        Subset *NewSubset;

    private:
        int NumberOfSubsets = 0;
};

class CycleBreaking
{
    public:
        CycleBreaking() {};
        ~CycleBreaking() {};
        bool ReadFile(const char *);
        void Process();
        bool WriteFile(const char *);

    private:
        int TotalRemovedWeight = 0;
        CreateGraph *NewGraph;
        vector<Edge*> VectorOfRemovedEdges;
};

#endif
