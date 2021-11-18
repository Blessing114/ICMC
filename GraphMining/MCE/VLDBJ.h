//
// Created by Lenovo on 2020/12/22.
//

#ifndef GRAPHMINING_VLDBJ_H
#define GRAPHMINING_VLDBJ_H

#include <stdio.h>
#include "../Data/DataProcess.h"
#include "IncrementalMCE.h"

class VLDBJ : public IncrementalMCE {
public:
    VLDBJ(string TEST_FILE_PATH,string TEST_DATA_NAME,vector<int> cursorVector)
    : IncrementalMCE( TEST_FILE_PATH,TEST_DATA_NAME,cursorVector)
    {};

    list<clock_t>  loadData();

    vector<set<int> > batchInsertUpdate(list<set<int> > incrementEdges);

    vector<set<int> > getDeleteCliques(vector<set<int> > addCliques, list<set<int> > incrementEdges, bool deletion, map<int,set<int> > originGraph);

    void updateCliqueResult(vector<set<int> > addCliques, vector<set<int> > deleteCliques);

private:
    map<int,set<int> > getCommonSubgraph(int u,int v, set<int> &cand){
        map<int,set<int> > subgraphNeighborMap;
        set<int> neighboeSet_u = neighborMap[u];
        set<int> neighboeSet_v = neighborMap[v];
        cand = DataProcess::getCommonSet(neighboeSet_u,neighboeSet_v);
        cand.insert(u);
        cand.insert(v);

        for(set<int>::iterator nsit=cand.begin();nsit!=cand.end();nsit++){
            int x = *nsit;
            set<int> newNeighboeSet_x;
            set<int> neighboeSet_x = neighborMap[x];
            for(set<int>::iterator nsxit=neighboeSet_x.begin();nsxit!=neighboeSet_x.end();nsxit++){
                int n=*nsxit;
                if(cand.find(n)!=cand.end()){
                    newNeighboeSet_x.insert(n);
                }
            }
            if(newNeighboeSet_x.size()>0){
                subgraphNeighborMap[x] = newNeighboeSet_x;
            }
        }
        cand.erase(u);
        cand.erase(v);

        return subgraphNeighborMap;
    };

};


#endif //GRAPHMINING_VLDBJ_H
