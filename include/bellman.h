#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
using namespace std;
#define MAX 1000

class BellmanFord {
    private:
        int graph[MAX][3];
        int vertexCount, edgeCount;
        vector<char> vertexList;
        vector<pair<int, int>>* adjacentList; 

        char start, end;
        int startIndex, endIndex;

        bool pathFound;
        vector<char> pathList;
    public:
        BellmanFord(int graph[MAX][3], int edgeCount, char start, char end);
        ~BellmanFord();
        int getIndex(char vertex);
        vector<char> createVertexList();
        vector<pair<int, int>>* createAdjacentList();
        void calculateStep(int inputCurrent[], int inputPrevious[]);
        void run();

        string path();
};

void BF(int graph[MAX][3], int edgeCount, char start, int currentArr[], int previousArr[]);
string BF_Path(int graph[MAX][3], int edgeCount, char start, char end);

#endif