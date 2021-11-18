//
// Created by Lenovo on 2020/12/31.
//

#include "PGM.h"
/**
 * Load graph data from a edge file, format: (-/+ )v u
 * @param filePath
 * @param cursorVector : record the batch incremental pattern of the data
 * @param incrementalType : The batch incremental type of the data: 0-mixture, 1-add, -1-delete
 */
list<clock_t> PGM::loadData(){
    list<clock_t> timeStamps;
    clock_t ts;
    string filePath = FILE_PATH1 + "clean/" + DATA_NAME1 + ".txt";
    ifstream dataFile(filePath.c_str());
    if(dataFile.is_open()){
        string line;
        int u,v;
        bool add = true;
        for(int i=0;i<CURSOR_VECTOR.size();i++) {
            ts = clock();
            timeStamps.push_back(ts);
            int cursor = CURSOR_VECTOR[i];
            set<int> updateVertex;
            for (int j = 0; j < cursor; j++) {
                if (getline(dataFile, line)) {
                    DataProcess::decodeEdgeLine(line, u, v, add);
                    set<int> neighbor_u, neighbor_v;
                    if(neighborMap.find(u)!=neighborMap.end()){
                        neighbor_u = neighborMap[u];
                    }
                    if(neighborMap.find(v)!=neighborMap.end()){
                        neighbor_v = neighborMap[v];
                    }
                    if(add==true){
                        neighbor_u.insert(v);
                        neighbor_v.insert(u);
                        neighborMap[u] = neighbor_u;
                        neighborMap[v] = neighbor_v;

                        updateVertex.insert(v);
                        updateVertex.insert(u);

                    }else{
                        neighbor_u.erase(v);
                        neighbor_v.erase(u);
                        if(neighbor_u.size()>0){
                            neighborMap[u] = neighbor_u;
                        }else{
                            neighborMap.erase(u);
                        }
                        if(neighbor_v.size()>0){
                            neighborMap[v] = neighbor_v;
                        } else{
                            neighborMap.erase(v);
                        }
                        deleteEdge(u,v);
                    }

                } else {
                    cout << "[WARNING]: Out of range: i=" << i << ", j=" << j << endl;
                    break;
                }
            }

            if(add==true){
                vector<set<int> > newCliques;
                list<set<int> > excludeEdges;
                set<int> K,cand,fini;
                map<int,set<int> > generatedSubgraph = getGenerateSubgraph(updateVertex, cand);
                tomita(newCliques, K, cand, fini, generatedSubgraph);

                set<string> cliqueResultIndex_new;
                for(set<string>::iterator crit=cliqueResultIndex.begin();crit!=cliqueResultIndex.end();crit++){
                    string clique_string = *crit;
                    set<int> old_clique = DataProcess::stringToClique(clique_string);
                    set<int> commonSet = DataProcess::getCommonSet(updateVertex,old_clique);
                    if(commonSet.size()==0){
                        cliqueResultIndex_new.insert(clique_string);
                    }
                }
                for(vector<set<int> >::iterator ncit=newCliques.begin();ncit!=newCliques.end();ncit++){
                    set<int> new_clique = *ncit;
                    set<int> commonSet = DataProcess::getCommonSet(updateVertex,new_clique);
                    if(commonSet.size()>0){
                        cliqueResultIndex_new.insert(DataProcess::cliqueToString(new_clique));
                    }
                }
                cliqueResultIndex = cliqueResultIndex_new;
            }

            if(WRITE_BATCH_RESULT == true) {
                int cliqueNumber = recordResult(DATA_NAME1 + "-"+std::to_string(BATCH_SIZE)+ "-PGM-" + std::to_string(i) + ".txt");
            }
        }

        ts = clock();
        timeStamps.push_back(ts);
        dataFile.close();
    }else{
        cout<<"Open File Error!"<<endl;
    }

    return timeStamps;
}

void PGM::deleteEdge(int u, int v){
    set<string> deleteCliques, addCliques;
    for(set<string>::iterator it = cliqueResultIndex.begin();it!=cliqueResultIndex.end();it++) {
        string C_string = *it;
        set<int> C = DataProcess::stringToClique(C_string);
        if(C.find(v)!=C.end() && C.find(u)!=C.end()){
            deleteCliques.insert(C_string);
            set<int> C1 = C;
            C1.erase(u);
            if(maxEval(C1)==true){
                addCliques.insert(DataProcess::cliqueToString(C1));
            }
            set<int> C2 = C;
            C2.erase(v);
            if(maxEval(C2)==true){
                addCliques.insert(DataProcess::cliqueToString(C2));
            }
        }
    }

    for(set<string>::iterator dcit=deleteCliques.begin();dcit!=deleteCliques.end();dcit++){
        string dc = *dcit;
        cliqueResultIndex.erase(dc);
    }

    for(set<string>::iterator acit=addCliques.begin();acit!=addCliques.end();acit++){
        string ac = *acit;
        cliqueResultIndex.insert(ac);
    }

}


bool PGM::maxEval(set<int> C){
    set<int> neighborSet;
    for(set<int>::iterator cit=C.begin();cit!=C.end();cit++){
        set<int> neighbors = neighborMap[*cit];
        for(set<int>::iterator nit=neighbors.begin();nit!=neighbors.end();nit++){
            if(C.find(*nit)==C.end()){
                neighborSet.insert(*nit);
            }
        }
    }
    for(set<int>::iterator nsit=neighborSet.begin();nsit!=neighborSet.end();nsit++){
        int nei = *nsit;
        set<int> nei_nei = neighborMap[nei];
        if(DataProcess::isSubSet(C,nei_nei)== true){
            return false;
        }
    }
    return true;
}