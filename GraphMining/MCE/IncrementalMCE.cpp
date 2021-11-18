//
// Created by Lenovo on 2020/12/31.
//

#include "IncrementalMCE.h"
void IncrementalMCE::tomita(vector<set<int> > &newCliques, set<int> K, set<int> cand, set<int> fini, map<int,set<int> > targetGraph) {
    if (cand.empty() && fini.empty()) {
        newCliques.push_back(K);
        return;
    }

    //get pivot
    int pivot = -1;
    int max_size = -1;
    for (set<int>::iterator cit = cand.begin(); cit != cand.end(); cit++) {
        set<int> neighbors = targetGraph[*cit];
        set<int> temp = DataProcess::getCommonSet(cand, neighbors);
        int num = temp.size();
        if (num > max_size) {
            max_size = num;
            pivot = *cit;
        }
    }
    for (set<int>::iterator fit = fini.begin(); fit != fini.end(); fit++) {
        set<int> neighbors = targetGraph[*fit];
        set<int> temp = DataProcess::getCommonSet(cand, neighbors);
        int num = temp.size();
        if (num > max_size) {
            max_size = num;
            pivot = *fit;
        }
    }
    if(pivot==-1){
        cout<<"[ERROR5]:"<<endl;
    }

    set<int> pivot_neighbor = targetGraph[pivot];
    set<int> ext = DataProcess::getDifferentSet(cand, pivot_neighbor);
    for(set<int>::iterator eit=ext.begin();eit!=ext.end();eit++){
        int q = *eit;
        set<int> K_q = K;
        K_q.insert(q);
        set<int> neighbor_q = targetGraph[q];
        set<int> cand_q = DataProcess::getCommonSet(cand,neighbor_q);
        set<int> fini_q = DataProcess::getCommonSet(fini,neighbor_q);
        tomita(newCliques,K_q,cand_q,fini_q,targetGraph);
        cand.erase(q);
        fini.insert(q);
    }
}

void IncrementalMCE::tomitaExcludeEdges(vector<set<int> > &newCliques, set<int> K, set<int> cand, set<int> fini, set<string> excludeEdges, map<int,set<int> > targetGraph) {
    if (cand.empty() && fini.empty()) {
        newCliques.push_back(K);
        return;
    }

    //get pivot
    int pivot = -1;
    int max_size = -1;
    for (set<int>::iterator cit = cand.begin(); cit != cand.end(); cit++) {
        set<int> neighbors = targetGraph[*cit];
        set<int> temp = DataProcess::getCommonSet(cand, neighbors);
        int num = temp.size();
        if (num > max_size) {
            max_size = num;
            pivot = *cit;
        }
    }
    for (set<int>::iterator fit = fini.begin(); fit != fini.end(); fit++) {
        set<int> neighbors = targetGraph[*fit];
        set<int> temp = DataProcess::getCommonSet(cand, neighbors);
        int num = temp.size();
        if (num > max_size) {
            max_size = num;
            pivot = *fit;
        }
    }
    if(pivot==-1){
        cout<<"[ERROR5]:"<<endl;
    }

    set<int> pivot_neighbor = targetGraph[pivot];
    set<int> ext = DataProcess::getDifferentSet(cand, pivot_neighbor);
    for(set<int>::iterator eit=ext.begin();eit!=ext.end();eit++){
        int q = *eit;
        set<int> K_q = K;
        K_q.insert(q);

        bool containEdge = false;
        for(set<int>::iterator kqit=K_q.begin();kqit!=K_q.end();kqit++){//faster than iterating edges
            int u = *kqit;
            set<int>::iterator kqit2=kqit;
            for(kqit2++;kqit2!=K_q.end();kqit2++){
                int v = *kqit2;
                string edge_str = DataProcess::edgeToString(u,v);
                if(excludeEdges.find(edge_str)!=excludeEdges.end()){
                    containEdge = true;
                    break;
                }
            }
            if(containEdge == true){
                break;
            }
        }
        if(containEdge == true){
            cand.erase(q);
            fini.insert(q);
            continue;
        }
        set<int> neighbor_q = targetGraph[q];
        set<int> cand_q = DataProcess::getCommonSet(cand,neighbor_q);
        set<int> fini_q = DataProcess::getCommonSet(fini,neighbor_q);
        tomitaExcludeEdges(newCliques,K_q,cand_q,fini_q,excludeEdges,targetGraph);
        cand.erase(q);
        fini.insert(q);
    }
}


int IncrementalMCE::recordResult(string suffix){
    int cliqueNumber = 0;
    ofstream cliqueFile;
    string filePath = FILE_PATH1 + "result/" + suffix;
    cliqueFile.open(filePath.c_str());
    if (cliqueFile.is_open()){
        cliqueNumber = (int)cliqueResultIndex.size();
        for(set<string>::iterator it = cliqueResultIndex.begin();it!=cliqueResultIndex.end();it++){
            string clique = *it;
            if(clique.find(" ") != clique.npos){
                cliqueFile<<clique<<"\n";
            }
        }
        cliqueFile.close();
    }

    return cliqueNumber;
}



bool IncrementalMCE::cliqueStringContain(string c_string, int x){
    string x_string = std::to_string(x);
    if(c_string.find(x_string)!=c_string.npos){
        return true;
    }else{
        return false;
    }
}


map<int,set<int> > IncrementalMCE::getGenerateSubgraph(set<int> vertices, set<int> &extendVertex){
    map<int,set<int> > subgraphNeighborMap;
    for(set<int>::iterator vit=vertices.begin();vit!=vertices.end();vit++){
        int v = *vit;
        set<int> neighboeSet_v = neighborMap[v];
        for(set<int>::iterator nit=neighboeSet_v.begin();nit!=neighboeSet_v.end();nit++){
            extendVertex.insert(*nit);
        }
        extendVertex.insert(v);
    }
    for(set<int>::iterator nsit=extendVertex.begin();nsit!=extendVertex.end();nsit++){
        int x = *nsit;
        set<int> newNeighboeSet_x;
        set<int> neighboeSet_x = neighborMap[x];
        for(set<int>::iterator nsxit=neighboeSet_x.begin();nsxit!=neighboeSet_x.end();nsxit++){
            int n=*nsxit;
            if(extendVertex.find(n)!=extendVertex.end()){
                newNeighboeSet_x.insert(n);
            }
        }
        if(newNeighboeSet_x.size()>0){
            subgraphNeighborMap[x] = newNeighboeSet_x;
        }
    }

    return subgraphNeighborMap;
}


bool IncrementalMCE::isMaximal(set<int> c, map<int,set<int> > originGraph){
    set<int>::iterator cit = c.begin();
    int init = *cit;
    set<int> commonNeighbors =originGraph[init];
    for(cit++;cit!=c.end();cit++){
        set<int> neighbors = originGraph[*cit];
        set<int> temp = DataProcess::getCommonSet(commonNeighbors,neighbors);
        commonNeighbors = temp;
    }
    if(commonNeighbors.size()>0){
        return false;
    }else{
        return true;
    }
}



void IncrementalMCE::getResultDifference(set<string> oldResult, set<string> newResult, int &changeCliqueNumber, int &changeVertexNumber, int &changeEdgeNumber){
    set<string> S3;
    set<string>::iterator it1 = oldResult.begin();
    set<string>::iterator it2 = newResult.begin();
    while (it1 != oldResult.end() && it2 != newResult.end()) {
        if (*it1 == *it2) {
            it1++;
            it2++;
        } else if (*it1 < *it2) {
            S3.insert(*it1);
            it1++;
        } else {
            S3.insert(*it2);
            it2++;
        }
    }
    while(it1 != oldResult.end()){
        S3.insert(*it1);
        it1++;
    }
    while(it2 != newResult.end()){
        S3.insert(*it2);
        it2++;
    }
    changeCliqueNumber = S3.size();
    for(set<string>::iterator sit3 = S3.begin();sit3!=S3.end();sit3++){
        string clique = *sit3;
        vector<string> nodes;
        DataProcess::split(clique," ",nodes);
        int N = nodes.size();
        changeVertexNumber += N;
        changeEdgeNumber += N*(N-1)/2;
    }
}


