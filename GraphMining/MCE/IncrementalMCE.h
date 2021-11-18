//
// Created by Lenovo on 2020/12/31.
//

#ifndef GRAPHMINING_INCREMENTALMCE_H
#define GRAPHMINING_INCREMENTALMCE_H

const bool DUBUG_SWITCH = false;
const bool RECORD_BATCH_DIFFERENCE = false;
const bool WRITE_BATCH_RESULT = false;

#include "../Data/DataProcess.h"

class IncrementalMCE {
public:
    IncrementalMCE();

    IncrementalMCE(string testFilePath,string testDataName,vector<int> cursorVector){
        FILE_PATH1 = testFilePath;
        DATA_NAME1 = testDataName;
        CURSOR_VECTOR = cursorVector;
    };

    string FILE_PATH1;
    string DATA_NAME1;
    vector<int> CURSOR_VECTOR;
    int BATCH_SIZE;

    map<int,set<int> > neighborMap;
    map<int,set<int> > smallNeighborMap;
    map<int,set<int> > bigNeighborMap;

    set<string> cliqueResultIndex;


    int getBatchSize() const {
        return BATCH_SIZE;
    }

    void setBatchSize(int batchSize) {
        BATCH_SIZE = batchSize;
    }

    void tomita(vector<set<int> > &newCliques, set<int> K, set<int> cand, set<int> fini, map<int,set<int> > targetGraph);

    void tomitaExcludeEdges(vector<set<int> > &newCliques, set<int> K, set<int> cand, set<int> fini, set<string> excludeEdges, map<int,set<int> > targetGraph);

    int recordResult(string suffix);

    bool cliqueStringContain(string c_string, int x);

    map<int,set<int> > getGenerateSubgraph(set<int> vertices, set<int> &extendVertex);

    bool isMaximal(set<int> c, map<int,set<int> > originGraph);

    void getResultDifference(set<string> oldResult, set<string> newResult, int &changeCliqueNumber, int &changeVertexNumber, int &changeEdgeNumber);


};


#endif //GRAPHMINING_INCREMENTALMCE_H
