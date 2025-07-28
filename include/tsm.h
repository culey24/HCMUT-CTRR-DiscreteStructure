#ifndef TSM_H
#define TSM_H

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
using namespace std;

#define MAX 1000

struct Node {
    int cost, bound, level;
    vector<int> path;
    vector<vector<int>> reducedMatrix;

    Node(int n) {
        reducedMatrix = vector<vector<int>>(n, vector<int>(n, -1));
        cost = 0;
        bound = 0;
        level = 0;
    }

    // Comparator for sorting
    bool operator<(const Node& other) const {
        return bound < other.bound;
    }
};

class Salesman {
    protected:
        int graph[MAX][3];
        int edgeCount, vertexCount;
        char startVertex;
        vector<char> vertexList;
        vector<int> bestPath;
        int** originalMatrix;

    public:
        Salesman(int input[MAX][3], int edgeCount, char startVertex);
        virtual ~Salesman();
        virtual void run() = 0;
        vector<char> getVertexList();
        string getBestPath();
        int getIndex(char vertex);
        int** createMatrix();      
};


class BranchAndBound : public Salesman {
    private:
    public:
        BranchAndBound(int input[MAX][3], int edgeCount, char startVertex);
        ~BranchAndBound() = default;
        void run() override;
        int reduceMatrix(vector<vector<int>>& matrix, int n);
};

class LKH : public Salesman {
    private:
        static int MAX_ITERATIONS;
        static int PENALTY;
    public:
        LKH(int input[MAX][3], int edgeCount, char startVertex);
        ~LKH() = default;
        void run() override;

        int computeTourCost(const vector<int>& path);
        bool applyTwoOpt(vector<int>& tour, int& bestCost);
        bool applyThreeOpt(std::vector<int>& tour, int& bestCost);
        vector<int> makeSimpleTour(bool toggle);
};

int getVertexCount(int graph[MAX][3], int edgeCount);
string Traveling(int graph[MAX][3], int edgeCount, char start);

#endif