//
// Created by Lenovo on 2020/12/31.
//

#ifndef GRAPHMINING_PGM_H
#define GRAPHMINING_PGM_H

#include "IncrementalMCE.h"

using namespace std;

class PGM : public IncrementalMCE {
public:
    PGM(string TEST_FILE_PATH, string TEST_DATA_NAME, vector<int> cursorVector)
    : IncrementalMCE( TEST_FILE_PATH,TEST_DATA_NAME,cursorVector)
    {};

    list<clock_t>  loadData();

    void deleteEdge(int u, int v);

    bool maxEval(set<int> C);
};


#endif //GRAPHMINING_PGM_H
