#include "tsm.h"

// SALESMAN

Salesman::Salesman(int input[MAX][3], int edgeCount, char startVertex) {
    this->edgeCount = edgeCount;
    this->startVertex = startVertex;
    for (int i = 0; i < edgeCount; i++) {
        graph[i][0] = input[i][0];
        graph[i][1] = input[i][1];
        graph[i][2] = input[i][2];
    }

    vertexList = getVertexList();
    vertexCount = vertexList.size();
    originalMatrix = createMatrix();
}

Salesman::~Salesman() {
    for (int i = 0; i < vertexCount; i++)
        delete[] originalMatrix[i];
    delete[] originalMatrix;
}

vector<char> Salesman::getVertexList() {
    vector<char> vertices;
    for (int i = 0; i < edgeCount; i++) {
        if (find(vertices.begin(), vertices.end(), static_cast<char>(graph[i][0])) == vertices.end())
            vertices.push_back(static_cast<char>(graph[i][0]));
        if (find(vertices.begin(), vertices.end(), static_cast<char>(graph[i][1])) == vertices.end())
            vertices.push_back(static_cast<char>(graph[i][1]));
    }
    sort(vertices.begin(), vertices.end());
    return vertices;
}

int Salesman::getIndex(char v) {
    auto it = find(vertexList.begin(), vertexList.end(), v);
    return (it != vertexList.end()) ? distance(vertexList.begin(), it) : -1;
}

int** Salesman::createMatrix() {
    int** result = new int*[vertexCount];
    for (int i = 0; i < vertexCount; i++) {
        result[i] = new int[vertexCount];
        for (int j = 0; j < vertexCount; j++)
            result[i][j] = -1;
    }

    for (int i = 0; i < edgeCount; i++) {
        int u = getIndex(graph[i][0]);
        int v = getIndex(graph[i][1]);
        result[u][v] = graph[i][2];
    }

    return result;
}

string Salesman::getBestPath() {
    if (bestPath.empty()) return "NO PATH FOUND";
    string result;
    for (int i = 0; i < bestPath.size(); i++) {
        result += vertexList[bestPath[i]];
        if (i != bestPath.size() - 1) result += ' ';
    }
    return result;
}

// BRANCH AND BOUND
BranchAndBound::BranchAndBound(int input[MAX][3], int edgeCount, char startVertex) : Salesman(input, edgeCount, startVertex){
}

int BranchAndBound::reduceMatrix(vector<vector<int>>& matrix, int n) {
    int reduction = 0;

    for (int i = 0; i < n; i++) {
        int rowMin = -1;

        for (int j = 0; j < n; j++) {
            if (matrix[i][j] != -1 && (rowMin == -1 || matrix[i][j] < rowMin)) {
                rowMin = matrix[i][j];
            }
        }

        if (rowMin > 0 && rowMin != -1) {
            for (int j = 0; j < n; j++) {
                if (matrix[i][j] != -1) {
                    matrix[i][j] -= rowMin;
                }
            }
            reduction += rowMin;
        }
    }

    for (int j = 0; j < n; j++) {
        int colMin = -1;
        for (int i = 0; i < n; i++) {
            if (matrix[i][j] != -1 && (colMin == -1 || matrix[i][j] < colMin))
                colMin = matrix[i][j];
        }
        if (colMin > 0 && colMin != -1) {
            for (int i = 0; i < n; i++) {
                if (matrix[i][j] != -1)
                    matrix[i][j] -= colMin;
            }
            reduction += colMin;
        }
    }

    return reduction;
}

void BranchAndBound::run() {
    int startIndex = getIndex(startVertex);
    int minCost = -1;
    vector<Node> searchQueue;

    // INITIALIZING ROOT 
    Node root(vertexCount);
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            root.reducedMatrix[i][j] = originalMatrix[i][j]; 
        }
    } // initializing root matrix   
    // reduce root matrix and calculate reduction
    root.bound = reduceMatrix(root.reducedMatrix, vertexCount); // bound = cost( = 0 at root) + reduction 
    root.path.push_back(startIndex);
    searchQueue.push_back(root);

    while (!searchQueue.empty()) {
        sort(searchQueue.begin(), searchQueue.end());

        Node currentNode = searchQueue.front();
        searchQueue.erase(searchQueue.begin());

        if (currentNode.level == vertexCount - 1) {
            int endIndex = currentNode.path.back();
            int returnCost = originalMatrix[endIndex][startIndex];
            if (returnCost != -1) {
                int total = currentNode.cost + currentNode.bound + returnCost;
                if (minCost == -1 || total < minCost) {
                    minCost = total;
                    bestPath = currentNode.path;
                    bestPath.push_back(startIndex);
                }
            }
            continue;
        }

        int currentCityIndex = currentNode.path.back();
        for (int i = 0; i < vertexCount; i++) {
            if (originalMatrix[currentCityIndex][i] != -1 && find(currentNode.path.begin(), currentNode.path.end(), i) == currentNode.path.end()) {
                Node childNode(vertexCount);
                childNode.reducedMatrix = currentNode.reducedMatrix;

                for (int j = 0; j < vertexCount; j++) {
                    childNode.reducedMatrix[currentCityIndex][j] = -1;
                    childNode.reducedMatrix[j][i] = -1;
                }
                childNode.reducedMatrix[i][startIndex] = -1; 

                childNode.path = currentNode.path;
                childNode.path.push_back(i);
                childNode.level = currentNode.level + 1;
                childNode.cost = currentNode.cost + currentNode.reducedMatrix[currentCityIndex][i];
                childNode.bound = childNode.cost + reduceMatrix(childNode.reducedMatrix, vertexCount);

                if (minCost == -1 || childNode.bound < minCost)
                    searchQueue.push_back(childNode);
            }
        }
    }
}
// Lin-Kernighan Heuristic

int LKH::MAX_ITERATIONS = 10;
int LKH::PENALTY = 999999;

LKH::LKH(int input[MAX][3], int edgeCount, char startVertex) : Salesman(input, edgeCount, startVertex){
}

vector<int> LKH::makeSimpleTour(bool toggle) {
    if (!toggle) return vector<int>();
    int startIndex = getIndex(startVertex);
    vector<int> tour;
    for (int i = 0; i < vertexCount; ++i) {
        tour.push_back(i);
    }
    auto it = find(tour.begin(), tour.end(), startIndex);
    if (it != tour.end()) {
        iter_swap(tour.begin(), it);
    }
    return tour;
}

int LKH::computeTourCost(const vector<int>& path) {
    int totalCost = 0;
    for (int i = 0; i < path.size() - 1; i++) {
        int cost = originalMatrix[path[i]][path[i + 1]];
        if (cost == -1) {
            totalCost += PENALTY;
        }
        else totalCost += cost;
    }

    int returnCost = originalMatrix[path.back()][path[0]];
    if (returnCost == -1) totalCost += PENALTY;
    else totalCost += returnCost;
    return totalCost;
}

bool LKH::applyTwoOpt(vector<int>& tour, int& bestCost) {
    bool improved = false;
    for (int i = 1; i < vertexCount - 2; ++i) {
        for (int j = i + 1; j < vertexCount - 1; ++j) {
            vector<int> candidate = tour;
            reverse(candidate.begin() + i, candidate.begin() + j + 1);

            int candidateCost = computeTourCost(candidate);
            // 
            if ((candidateCost < bestCost)) {
                tour = candidate;
                bestCost = candidateCost;
                improved = true;
            }
        }
    }
    return improved;
}

bool LKH::applyThreeOpt(std::vector<int>& tour, int& bestCost) {
    bool improved = false;

    for (int i = 1; i < vertexCount - 2; ++i) {
        for (int j = i + 1; j < vertexCount - 1; ++j) {
            for (int k = j + 1; k < vertexCount; ++k) {
                std::vector<std::vector<int>> candidates;

                // Move 1: reverse segment i to j
                std::vector<int> c1 = tour;
                std::reverse(c1.begin() + i, c1.begin() + j + 1);
                candidates.push_back(c1);

                // Move 2: reverse segment j+1 to k
                std::vector<int> c2 = tour;
                std::reverse(c2.begin() + j + 1, c2.begin() + k + 1);
                candidates.push_back(c2);

                // Move 3: reverse both segments
                std::vector<int> c3 = tour;
                std::reverse(c3.begin() + i, c3.begin() + j + 1);
                std::reverse(c3.begin() + j + 1, c3.begin() + k + 1);
                candidates.push_back(c3);

                for (auto& candidate : candidates) {
                    int newCost = computeTourCost(candidate);
                    // 
                    if ((newCost < bestCost)) {
                        tour = candidate;
                        bestCost = newCost;
                        improved = true;
                    }
                }
            }
        }
    }

    return improved;
}

void LKH::run() {
    int startIndex = getIndex(startVertex);
    
    if (startIndex == -1 || vertexCount <= 0) {
        return; // Invalid start vertex or no vertices
    }

    bool tourCondition = true; // Check connectivity
    for (int i = 0; i < vertexCount; i++) {
        bool hasOutgoing = false;
        for (int j = 0; j < vertexCount; j++) {
            if (i != j && originalMatrix[i][j] != -1) {
                hasOutgoing = true;
                break;
            }
        }
        if (!hasOutgoing) {
            tourCondition = false;
            break;
        }
    }
    
    if (!tourCondition) { // No tour can found, traveling salesman lost his job :(, anw make a place holder path.
        bestPath = makeSimpleTour(true);
        bestPath.push_back(bestPath[0]);
        return;
    }

    // Create initial tour using nearest neighbor + opt
    vector<int> tour;
    vector<bool> visited(vertexCount, false);
    
    tour.push_back(startIndex);
    visited[startIndex] = true;
    
    // Build tour using nearest neighbor with fallback
    for (int count = 1; count < vertexCount; count++) {
        int current = tour.back();
        int nearest = -1;
        int minDist = -1;
        
        // First try: find nearest unvisited neighbor
        for (int i = 0; i < vertexCount; i++) {
            if (!visited[i] && originalMatrix[current][i] != -1) {
                if (minDist == -1 || originalMatrix[current][i] < minDist) {
                    minDist = originalMatrix[current][i];
                    nearest = i;
                }
            }
        }
        
        // Second try: if no direct connection, find any unvisited node
        if (nearest == -1) {
            for (int i = 0; i < vertexCount; i++) {
                if (!visited[i]) {
                    nearest = i;
                    break;
                }
            }
        }

        if (nearest != -1) {
            tour.push_back(nearest);
            visited[nearest] = true;
        } 
        else {
            break;
        }
    }
    
    // Ensure we have a complete tour
    if (tour.size() != vertexCount) {
        // Add missing vertices
        for (int i = 0; i < vertexCount; i++) {
            if (find(tour.begin(), tour.end(), i) == tour.end()) {
                tour.push_back(i);
            }
        }
    }

    // Always try to create some tour, even if not optimal
    int initialCost = computeTourCost(tour);
    //  || 
    if (initialCost == -1 || tour.empty() || tour.size() != vertexCount) {
        tour.clear();
        tour = makeSimpleTour(true);
        initialCost = computeTourCost(tour);
    }

    int bestCost = initialCost;
    
    // Apply 2-opt first (faster)
    bool improved = true;
    while (improved) {
        improved = applyTwoOpt(tour, bestCost);
    }
    
    // Then apply 3-opt for further improvement
    improved = true;
    int iterations = 0;
    
    while (improved && iterations < MAX_ITERATIONS) {
        improved = applyThreeOpt(tour, bestCost);
        iterations++;
    }

    tour.push_back(tour[0]); 
    bestPath = tour;
}

int getVertexCount(int graph[MAX][3], int edgeCount) {
    vector<char> vertices;
    for (int i = 0; i < edgeCount; i++) {
        if (find(vertices.begin(), vertices.end(), static_cast<char>(graph[i][0])) == vertices.end())
            vertices.push_back(static_cast<char>(graph[i][0]));
        if (find(vertices.begin(), vertices.end(), static_cast<char>(graph[i][1])) == vertices.end())
            vertices.push_back(static_cast<char>(graph[i][1]));
    }
    if (vertices.empty()) return -1;
    return vertices.size();
}

string Traveling(int graph[MAX][3], int edgeCount, char start) {
    int vertexCount = getVertexCount(graph, edgeCount);
    Salesman* tsp;
    if (vertexCount <= 0) return "INVALED VERTEX COUNT";
    else if (vertexCount < 20) tsp = new BranchAndBound(graph, edgeCount, start);
    else tsp = new LKH(graph, edgeCount, start);
    tsp->run();

    string result = tsp->getBestPath();
    delete tsp;
    return result;
}