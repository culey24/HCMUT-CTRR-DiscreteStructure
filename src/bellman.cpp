#include "bellman.h"

BellmanFord::BellmanFord(int input[MAX][3], int edgeCount, char start, char end) : edgeCount(edgeCount), start(start), end(end), pathFound(false) {
    if (input) {
        for (int i = 0; i < edgeCount; i++) {
            graph[i][0] = input[i][0];
            graph[i][1] = input[i][1];
            graph[i][2] = input[i][2];
        }
    }

    vertexList = createVertexList();
    vertexCount = vertexList.size();

    startIndex = getIndex(start);
    endIndex = getIndex(end);

    adjacentList = createAdjacentList();
}

BellmanFord::~BellmanFord() {
    delete[] adjacentList;
}

int BellmanFord::getIndex(char vertex) {
    auto pos = find(vertexList.begin(), vertexList.end(), vertex);
    if (pos != vertexList.end()) {
        return distance(vertexList.begin(), pos);
    }
    return -1;
}

vector<char> BellmanFord::createVertexList() {
    vector<char> vertexList;
    for (int i = 0; i < edgeCount; i++) {
        if (find(vertexList.begin(), vertexList.end(), static_cast<char>(graph[i][0])) == vertexList.end()) {
            vertexList.push_back(static_cast<char>(graph[i][0]));
        }
        if (find(vertexList.begin(), vertexList.end(), static_cast<char>(graph[i][1])) == vertexList.end()) {
            vertexList.push_back(static_cast<char>(graph[i][1]));
        }
    }
    sort(vertexList.begin(), vertexList.end());
    return vertexList;
}

vector<pair<int, int>>* BellmanFord::createAdjacentList() {
    vector<pair<int, int>>* adjacentVertices = new vector<pair<int, int>>[vertexCount];
    
    // u_index -> list of pair(v_index, weight)

    for (int i = 0; i < edgeCount; i++) {
        char u = static_cast<char>(graph[i][0]);
        char v = static_cast<char>(graph[i][1]);
        int edgeWeight = graph[i][2];

        int u_index = getIndex(u);
        int v_index = getIndex(v);

        adjacentVertices[u_index].push_back({v_index, edgeWeight});
    }
    return adjacentVertices;
}

void BellmanFord::calculateStep(int currentArr[], int previousArr[]) { 
    if (startIndex == -1) {
        previousArr[vertexCount - 1] = 0;
        return;
    }

    currentArr[startIndex] = 0;
    int tempCurrentArr[vertexCount];
    for (int i = 0; i < vertexCount; i++) {
        tempCurrentArr[i] = currentArr[i];
    }

    int tempPreviousArr[vertexCount];
    for (int i = 0; i < vertexCount; i++) {
        tempPreviousArr[i] = previousArr[i];
    }
    
    for (int uIndex = 0; uIndex < vertexCount; uIndex++) {
        if (tempCurrentArr[uIndex] == -1) continue;
        for (const auto& temp : adjacentList[uIndex]) {
            int vIndex = temp.first;
            int weight = temp.second;

            if (tempCurrentArr[vIndex] == -1 || tempCurrentArr[uIndex] + weight < tempCurrentArr[vIndex]) {
                if (currentArr[vIndex] == -1 || tempCurrentArr[uIndex] + weight < currentArr[vIndex]) {
                    currentArr[vIndex] = tempCurrentArr[uIndex] + weight;
                    previousArr[vIndex] = uIndex;
                }
            }
        }
    }
}

void BF(int graph[MAX][3], int edgeCount, char start, int currentArr[], int previousArr[]) {
    BellmanFord bellmanFord = BellmanFord(graph, edgeCount, start, 0);
    currentArr[bellmanFord.getIndex(start)] = 0;
    bellmanFord.calculateStep(currentArr, previousArr);
}   

string BellmanFord::path() {
    if (!pathFound) return "No path exists";

    string result;
    for (char vertex : pathList) {
        result += vertex;
        result += ' ';
    }
    if (!result.empty()) result.pop_back();
    return result;
}

void BellmanFord::run() {
    int currentArr[vertexCount];
    int previousArr[vertexCount];

    for (int i = 0; i < vertexCount; i++) {
        currentArr[i] = -1;
        previousArr[i] = -1;
    }

    currentArr[startIndex] = 0;

    for (int i = 0; i < vertexCount - 1; i++) {
        calculateStep(currentArr, previousArr);
    }

    int tempArr[vertexCount], tempPrev[vertexCount];
    copy(currentArr, currentArr + vertexCount, tempArr);
    copy(previousArr, previousArr + vertexCount, tempPrev);
    calculateStep(tempArr, tempPrev);

    for (int i = 0; i < vertexCount; i++) {
        if (currentArr[i] != tempArr[i]) {
            pathFound = false;
            throw "Negative weight cycle detected";
        }
    }

    if (currentArr[endIndex] == -1) {
        pathFound = false;  
        return;
    }

    for (int i = endIndex; i != -1; i = previousArr[i]) {
        pathList.push_back(vertexList[i]);
    }
    reverse(pathList.begin(), pathList.end());
    pathFound = true;
    return;
}

string BF_Path(int graph[MAX][3], int edgeCount, char start, char end) {
    BellmanFord bellmanFord(graph, edgeCount, start, end);
    bellmanFord.run();
    return bellmanFord.path();
}

