//
// Created by Lenovo on 2020/12/28.
//

#ifndef GRAPHMINING_ICDE_H
#define GRAPHMINING_ICDE_H

#include "../Data/DataProcess.h"
#include "IncrementalMCE.h"

using namespace std;

class ICDE : public IncrementalMCE{
public:
    ICDE(string TEST_FILE_PATH, string TEST_DATA_NAME, vector<int> cursorVector)
        : IncrementalMCE( TEST_FILE_PATH,TEST_DATA_NAME,cursorVector)
    {};

    list<clock_t>  loadData();

    void insertEdge(int p, int q);

    void deleteEdge(int u, int v);

    bool maxEval(set<int> cand);

};

#endif //GRAPHMINING_ICDE_H
