//
// Created by Lenovo on 2021/1/3.
//

#ifndef GRAPHMINING_COA_H
#define GRAPHMINING_COA_H

#include "IncrementalMCE.h"

class COA : public IncrementalMCE {
public:
    COA(string TEST_FILE_PATH,string TEST_DATA_NAME,vector<int> cursorVector)
    : IncrementalMCE( TEST_FILE_PATH,TEST_DATA_NAME,cursorVector)
    {};

    list<clock_t>  loadData();

    void insertEdge(int u, int v);

    void deleteEdge(int u, int v);

    bool maxEval(set<int> C);
};


#endif //GRAPHMINING_COA_H
