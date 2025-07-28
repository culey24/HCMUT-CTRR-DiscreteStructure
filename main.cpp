#include "bellman.h"
#include "tsm.h"

void printedge(int[]);
int edgeListGen(int[][3],int,int,int);

int main()
{
    bool randGen=0; // 1 TO TOGGLE RANDOM MODE, 0 TO TOGGLE USER-ASSIGN VALUE MODE.
    int edgeList[MAX][3] ;
    int numEdges=350;
    int numVertices=20;
    int initlimit=15;
    if(!randGen){
        string line;
        vector<string> data;
        std::ifstream file("EdgeList.txt");
        while (getline(file, line)) {
            data.push_back(line);
        }
        file.close();

        
        std::ifstream fin("EdgeList.txt");
        for(int i=0;i<data.size();i++){
            fin>>edgeList[i][0]>>edgeList[i][1]>>edgeList[i][2];
        }
        fin.close();
    }
    else{
        if(edgeListGen(edgeList,numEdges,numVertices,initlimit)<0) {
            std::cerr << "Error generating edge list." << std::endl;
            return -1;
        }
    }

    std::cout << "Generated Edge List (" << numEdges << " edges, " << numVertices << " vertices, weights up to " << initlimit << "):" << std::endl;
    for(int i=0; i<numEdges; i++){
        printedge(edgeList[i]);
    }
    std::cout << "-------------------------------------" << std::endl;

    char startingVertex = edgeList[0][0];
    char endingVertex = edgeList[numEdges - 1][1];

    int SELECTION;
    std::cout << "Select the algorithm to test:" << std::endl;
    std::cout << "1: Bellman-Ford (BF_Path)" << std::endl;
    std::cout << "2: Traveling Salesman (TSP)" << std::endl;
    std::cout << "3: Test Global BF function" << std::endl;
    std::cout << "Enter your selection (1, 2, or 3): ";
    std::cin >> SELECTION;
    std::cout << "-------------------------------------" << std::endl;

    switch (SELECTION) {
        case 1: {
            std::cout << "Running Bellman-Ford (BF_Path)..." << std::endl;
            std::cout << "Finding shortest path from " << startingVertex << " to " << endingVertex << std::endl;
            try {
                std::string path_result = BF_Path(edgeList, numEdges, startingVertex, endingVertex);
                std::cout << "Bellman-Ford Result: " << path_result << std::endl;
            } catch (const char* msg) {
                std::cerr << "Exception in Bellman-Ford: " << msg << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Exception in Bellman-Ford: " << e.what() << std::endl;
            }
            break;
        }
        case 2: {
            std::cout << "Running Traveling Salesman (TSP)..." << std::endl;
            std::cout << "Finding shortest tour starting from " << startingVertex << std::endl;
            try {
                Traveling(edgeList, numEdges, startingVertex);
            } catch (const char* msg) {
                std::cerr << "Exception in TSP: " << msg << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Exception in TSP: " << e.what() << std::endl;
            }
            break;
        }
        case 3: {
            std::cout << "Running Global BF function test..." << std::endl;
            std::cout << "Testing global BF function with start: " << startingVertex << std::endl;
            
            std::vector<char> tempVertexList;
            for (int i = 0; i < numEdges; i++) {
                bool found0 = false;
                bool found1 = false;
                for(char c : tempVertexList) {
                    if (c == static_cast<char>(edgeList[i][0])) found0 = true;
                    if (c == static_cast<char>(edgeList[i][1])) found1 = true;
                }
                if (!found0) tempVertexList.push_back(static_cast<char>(edgeList[i][0]));
                if (!found1) tempVertexList.push_back(static_cast<char>(edgeList[i][1]));
            }
            int vCount = tempVertexList.size();
            if (vCount == 0 && numEdges > 0) {
                 std::cout << "Warning: 0 vertices found from edges for BF test." << std::endl;
                 vCount = 1;
            } else if (vCount == 0) {
                 std::cout << "Graph is empty, cannot run BF test." << std::endl;
                 break;
            }

            std::vector<int> BFValue(vCount, -1);
            std::vector<int> BFPrev(vCount, -1);
            
            int sIdx = -1;
            for(int i=0; i < vCount; ++i) if(tempVertexList[i] == startingVertex) sIdx = i;
            
            if (sIdx != -1) {
                BFValue[sIdx] = 0;
            }

            BF(edgeList, numEdges, startingVertex, BFValue.data(), BFPrev.data());

            std::cout << "Global BF - currentArr (Distances):" << std::endl;
            for (int i = 0; i < vCount; i++) {
                std::cout << "Vertex " << (tempVertexList.empty() ? '?' : tempVertexList[i]) << " (idx " << i << "): " << BFValue[i] << std::endl;
            }
            std::cout << "Global BF - previousArr (Predecessors):" << std::endl;
            for (int i = 0; i < vCount; i++) {
                std::cout << "Vertex " << (tempVertexList.empty() ? '?' : tempVertexList[i]) << " (idx " << i << "): " << BFPrev[i] << std::endl;
            }
            break;
        }
        default:
            std::cout << "UNDEFINED SELECTION" << std::endl;
    }

    std::cout << "-------------------------------------" << std::endl;
    std::cout << "Test finished." << std::endl;
    
    return 0;
}

void printedge(int edge[]){
    std::cout << static_cast<char>(edge[0]) << "-" << static_cast<char>(edge[1]) << ", weight:" << edge[2] << std::endl;
}

int edgeListGen(int edgeList[][3],int numEdges,int numVertices,int initlimit){
    std::random_device rd;
    std::mt19937 gen(rd());
    
    if (numVertices <= 0) {
        std::cout << "Number of vertices must be positive." << std::endl;
        return -1;
    }
    if (numEdges < 0) {
        std::cout << "Number of edges cannot be negative." << std::endl;
        return -1;
    }
    if (numEdges > numVertices * (numVertices -1) && numVertices > 1) {
        std::cout << "Requested number of edges exceeds maximum possible for a simple directed graph without self-loops." << std::endl;
    }
     if (numVertices == 1 && numEdges > 0) {
        std::cout << "Cannot have edges with only one vertex if no self-loops." << std::endl;
        return -1;
    }

    if (numVertices > (126 - 33 + 1)) {
        std::cout << "Too many vertices requested for the character range." << std::endl;
        return -1;
    }
    std::vector<int> verNameCandidates;
    for(int i=33; i<=126; i++){
        verNameCandidates.push_back(i);
    }
    std::shuffle(verNameCandidates.begin(), verNameCandidates.end(), gen);
    
    std::vector<int> verList(numVertices);
    for(int i=0; i<numVertices; i++){
        verList[i] = verNameCandidates[i];
    }
    
    std::vector<std::pair<int,int>> fullEdgePossibleList;
    if (numVertices > 1) {
        for(int i=0; i<numVertices; i++){
            for(int j=0; j<numVertices; j++){
                if(i==j) continue;
                fullEdgePossibleList.push_back({verList[i], verList[j]});
            }
        }
    }

    if (numEdges > fullEdgePossibleList.size() && !fullEdgePossibleList.empty()) {
        std::cout << "Warning: Requested more edges than possible unique directed edges. Duplicates might occur or fewer edges will be generated." << std::endl;
    }
     if (fullEdgePossibleList.empty() && numEdges > 0) {
        std::cout << "Cannot generate edges, not enough vertices for distinct pairs." << std::endl;
        return -1;
    }

    std::shuffle(fullEdgePossibleList.begin(), fullEdgePossibleList.end(), gen);    
    
    int edgesToGenerate = std::min(numEdges, (int)fullEdgePossibleList.size());
    if (numEdges > edgesToGenerate) {
        std::cout << "Warning: Generating " << edgesToGenerate << " unique edges as it's the max possible. Requested " << numEdges << std::endl;
    }

    for(int i=0; i<edgesToGenerate; i++){
        edgeList[i][0] = fullEdgePossibleList[i].first;
        edgeList[i][1] = fullEdgePossibleList[i].second;
    }
    if (numEdges > edgesToGenerate && !fullEdgePossibleList.empty()) {
        std::uniform_int_distribution<int> dist_edge_idx(0, fullEdgePossibleList.size() - 1);
        for (int i = edgesToGenerate; i < numEdges; ++i) {
            int rand_idx = dist_edge_idx(gen);
            edgeList[i][0] = fullEdgePossibleList[rand_idx].first;
            edgeList[i][1] = fullEdgePossibleList[rand_idx].second;
        }
    } else if (numEdges == 0) {
    } else if (fullEdgePossibleList.empty() && numEdges > 0) {
    }

    std::ofstream fout("EdgeList.txt");
    if(initlimit<=1){
        for(int i=0;i<numEdges;i++){
            edgeList[i][2]=1;
            if (numVertices > 0)
                 fout<<static_cast<int>(edgeList[i][0])<<" "<<static_cast<char>(edgeList[i][1])<<" "<<edgeList[i][2]<<std::endl;
        }
    }
    else{
        std::uniform_int_distribution<int> dist_weight(1,initlimit);
        for(int i=0;i<numEdges;i++){
            edgeList[i][2]=dist_weight(gen);
            if (numVertices > 0)
                 fout<<static_cast<int>(edgeList[i][0])<<" "<<static_cast<char>(edgeList[i][1])<<" "<<edgeList[i][2]<<std::endl;
        }
    }
    fout.close();
    return 1;
}