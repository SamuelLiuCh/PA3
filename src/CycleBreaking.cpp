// **************************************************************************
//  File       [CycleBreaking.cpp]
//  Author     [Samuel Liu]
//  Synopsis   [CycleBreaking and CreateGraph and DisjoinSet]
//  Modify     [2022/01/11 Samuel Liu]
// **************************************************************************

#include "CycleBreaking.h"
using namespace std;

// **************************************************************************
// class of CycleBreaking
// **************************************************************************

bool GraphIsDirected = false;

bool CycleBreaking::ReadFile(const char *filename)
{
    //open input file
    //read the First Line u or d
    ifstream fin(filename);
    char GraphType;
    fin >> GraphType;
    if(GraphType == 'd')
        GraphIsDirected = true;
    //read the Second Line numbers of Vectors
    int NumbersOfVertices;
    fin >> NumbersOfVertices;
    //read the Third Line numbers of Edges
    int NumbersOfEdges;
    fin >> NumbersOfEdges;

    //create a Graph
    NewGraph = new CreateGraph(NumbersOfVertices, NumbersOfEdges, GraphIsDirected);
    for (int i = 0; i < NumbersOfEdges; i++)
    {
        int a, b, c;
        fin >> a >> b >> c;
        NewGraph->AddEdge(a, b, c);
    }
    //close input file
    fin.close();
    return true;
}

void CycleBreaking::Process()
{
    //do Main Process
    if(GraphIsDirected == true)
    {
        //do MFASP by DFS if Graph is Directed
        NewGraph->MFASPbyDFS(VectorOfRemovedEdges);
    }
    else
    {
        //do Kruskal MST if Graph is Undirected
        NewGraph->KruskalMST(VectorOfRemovedEdges);
    }

    //add total Removed Weight
    for (int i = 0; i < VectorOfRemovedEdges.size(); i++)
        TotalRemovedWeight += VectorOfRemovedEdges[i]->weight;
}

bool CycleBreaking::WriteFile(const char *filename)
{
    //open output file
    ofstream fout(filename);
    //check the Number of Removed Edges
    if(VectorOfRemovedEdges.empty() == true)
        fout << "0" << endl;
    else
    {
        fout << TotalRemovedWeight << endl;
        for(int i = 0; i < VectorOfRemovedEdges.size(); i++)
            fout << VectorOfRemovedEdges[i]->u << " " << VectorOfRemovedEdges[i]->v << " " << VectorOfRemovedEdges[i]->weight << "\n";
    }
    //close output file
    fout.flush();
    fout.close();
    
    delete NewGraph;
    return true;
}

// **************************************************************************
// class of CreateGraph
// **************************************************************************

CreateGraph::CreateGraph(int nv, int ne, bool dir) : NV(nv), NE(ne), DIR(dir)
{
    if (GraphIsDirected == true)
        ListOfAdjacentPairs = new list<AdjacentPairs>[nv];
    VectorOfEdge.reserve(ne);
}

CreateGraph::~CreateGraph()
{
    if (GraphIsDirected == true)
        delete[] ListOfAdjacentPairs;
}

void CreateGraph::DFS(const AdjacentPairs &adjp, int p, int *color, bool *IsInCycle, vector<AdjacentPairs> &parents, vector<Edge*> &VectorOfRemovedEdges, bool *ShouldAddEdge)
{
    int u = adjp.first;
    color[u] = GRAY;
    parents[u] = make_pair(p, adjp.second);

    for (auto i = ListOfAdjacentPairs[u].begin(); i != ListOfAdjacentPairs[u].end(); i++)
    {
        int v = (*i).first;    
        if (IsInCycle[v] == true)
        {
            continue;
        }
        
        switch(color[v])
        {
        case GRAY:
            DirRemoveEdgeE(v, u, (*i).second, IsInCycle, parents, VectorOfRemovedEdges, ShouldAddEdge);
            parents.clear();
        case WHITE:
            DFS(*i, u, color, IsInCycle, parents, VectorOfRemovedEdges, ShouldAddEdge);
        }
    }
    color[u] = BLACK;
}

void CreateGraph::MFASPbyDFS(vector<Edge*> &VectorOfRemovedEdges)
{
    vector<Edge*> tmpRemovedEdges;
    KruskalMST(tmpRemovedEdges);

    // remove Edges in Adjacent Pairs List from the Original Graph
    for (int i = 0; i < tmpRemovedEdges.size(); i++)
    {
        Edge e = *tmpRemovedEdges[i];
        list<AdjacentPairs>::iterator Iter = std::find(ListOfAdjacentPairs[e.u].begin(), ListOfAdjacentPairs[e.u].end(), make_pair(e.v, e.weight));
        ListOfAdjacentPairs[e.u].erase(Iter);
    }

    // add Edges back to the Graph and check if there exists a Cycle
    for (int i = 0; i < tmpRemovedEdges.size(); i++)
    {
        if ((*tmpRemovedEdges[i]).weight <= 0)
        {
            VectorOfRemovedEdges.push_back(tmpRemovedEdges[i]);
            continue;
        }

        ListOfAdjacentPairs[(*tmpRemovedEdges[i]).u].push_back(make_pair((*tmpRemovedEdges[i]).v, (*tmpRemovedEdges[i]).weight));
        if (HaveCycle() == true)
        {
            ListOfAdjacentPairs[(*tmpRemovedEdges[i]).u].pop_back();
            VectorOfRemovedEdges.push_back(tmpRemovedEdges[i]);
        }
    }
}

void CreateGraph::KruskalMST(vector<Edge*> &VectorOfRemovedEdges)
{
    // create Disjoin Set
    DisjoinSet set(NV);
    sort(VectorOfEdge.begin(), VectorOfEdge.end());

    for (int i = VectorOfEdge.size() - 1; i >= 0; --i)
    {
        if (set.FindSet(VectorOfEdge[i].u) != set.FindSet(VectorOfEdge[i].v))
            set.Union(VectorOfEdge[i].u, VectorOfEdge[i].v);
        else
            VectorOfRemovedEdges.push_back(&VectorOfEdge[i]);
    }
}

void CreateGraph::AddEdge(int &u, int &v, int &w)
{
    //build List of Adjacent Pairs
    if (GraphIsDirected == true)
        ListOfAdjacentPairs[u].push_back(make_pair(v, w));
    VectorOfEdge.push_back(Edge{u, v, w});
}

//check if there exists a Cycle
bool CreateGraph::HaveCycle() const
{
    int color[NV] = {WHITE};
    for (int v = 0; v < int(NV); v++)
    {
        if(color[v] == WHITE)
        {
            if (HaveCycleDFS(v, color) == true)
                return true;
        }
    }
    return false;
}

bool CreateGraph::HaveCycleDFS(int &u, int *color) const
{
    color[u] = GRAY;
    for (auto i = ListOfAdjacentPairs[u].begin(); i != ListOfAdjacentPairs[u].end(); i++)
    {
        int v = (*i).first;
        if (color[v] == GRAY)
            return true;
        else if (color[v] == WHITE && HaveCycleDFS(v, color))
            return true;
    }
    color[u] = BLACK;
    return false;
}

void CreateGraph::DirRemoveEdgeE(int &CycleStart, int &CycleEnd, int &LastWeight, bool *IsInCycle, vector<AdjacentPairs> &parents, vector<Edge *> &VectorOfRemovedEdges, bool *ShouldAddEdge)
{
    vector<Edge> CycleOfEdgeE;
    Edge EndOfEdgeE{CycleEnd, CycleStart, LastWeight};
    Edge MinOfEdgeE{0, 0, 0};

    // trace back the Edges in the Cycle
    IsInCycle[CycleEnd] = IsInCycle[CycleStart] = true;

    CycleOfEdgeE.push_back(EndOfEdgeE);
    for (int v = CycleEnd; v != CycleStart; v = parents[v].first)
    {
        IsInCycle[v] = true;
        Edge e{parents[v].first, v, parents[v].second};
        CycleOfEdgeE.push_back(e);
    }

    // FindSet Minimum Weight in Edge List
    for (int i = 0; i < CycleOfEdgeE.size(); i++)
    {
        if (CycleOfEdgeE[i] < MinOfEdgeE)
            MinOfEdgeE = CycleOfEdgeE[i];
    }

    // FindSet Minimum Edge in Edge List
    for (int i = 0; i < VectorOfEdge.size(); i++)
    {
        if (VectorOfEdge[i] == MinOfEdgeE && !ShouldAddEdge[i])
        {
            VectorOfRemovedEdges.push_back(&VectorOfEdge[i]);
            ShouldAddEdge[i] = true;
        }
    }
}

void CreateGraph::DirFindCycle(vector<Edge*> &VectorOfRemovedEdges)
{
    vector<AdjacentPairs> parents(NV);

    for (int v = 0; v < NV; v++)
    {
        bool IsInCycle[NV] = {false};
        bool ShouldAddEdge[NE] = {false};
        int  color[NV] = {0};
        DFS(make_pair(v, 0), -1, color, IsInCycle, parents, VectorOfRemovedEdges, ShouldAddEdge);
    }
}

// **************************************************************************
// class of DisjoinSet
// **************************************************************************

DisjoinSet::DisjoinSet(int &n) : NumberOfSubsets(n)
{
    NewSubset = new Subset[n];
    MakeSet();
}

DisjoinSet::~DisjoinSet()
{
    delete[] NewSubset;
}

void DisjoinSet::MakeSet()
{
    for (int i = 0; i < NumberOfSubsets; i++)
    {
        NewSubset[i].parent = i;
        NewSubset[i].rank = 0;
    }
}

int DisjoinSet::FindSet(int &i) const
{
    if (NewSubset[i].parent == i)
        return i;
    else
        return FindSet(NewSubset[i].parent);
}

void DisjoinSet::Union(int &i, int &j)
{
    int i_root = FindSet(i);
    int j_root = FindSet(j);
    if (NewSubset[i_root].rank < NewSubset[j_root].rank)
        NewSubset[i_root].parent = j_root;
    else if (NewSubset[i_root].rank > NewSubset[j_root].rank)
        NewSubset[j_root].parent = i_root;
    else
    {
        NewSubset[j_root].parent = i_root;
        NewSubset[i_root].rank ++;
    }
}