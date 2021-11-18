//
// Created by Lenovo on 2020/12/22.
//

#include "VLDBJ.h"

/**
 * Load graph data from a edge file, format: (-/+ )v u
 * @param filePath
 * @param cursorVector : record the batch incremental pattern of the data
 * @param incrementalType : The batch incremental type of the data: 0-mixture, 1-add, -1-delete
 */
list<clock_t> VLDBJ::loadData(){
    list<clock_t> timeStamps;
    clock_t ts;
    string filePath = FILE_PATH1 + "clean/" + DATA_NAME1 + ".txt";
    ifstream dataFile(filePath.c_str());
    if(dataFile.is_open()){
        string line;
        int u,v;
        bool add = true;
        for(int i=0;i<CURSOR_VECTOR.size();i++){
            ts = clock();
            timeStamps.push_back(ts);
            int cursor = CURSOR_VECTOR[i];
            list<set<int> > incrementEdges,decreaseEdges;
            set<string> incrementEdgeSet,decreaseEdgeSet;
            map<int,set<int> > originGraph = neighborMap;
            for(int j=0;j<cursor;j++){
                if(getline(dataFile,line)){
                    DataProcess::decodeEdgeLine(line,u,v,add);
                    if(add==true){
                        string edge_string = std::to_string(u) + "\t" + std::to_string(v);
                        if(incrementEdgeSet.find(edge_string)!=incrementEdgeSet.end()){
                            if(decreaseEdgeSet.find(edge_string)!=decreaseEdgeSet.end()){
                                decreaseEdgeSet.erase(edge_string);
                            }else{
                                cout<<"[ERROR] randance of edge insertion : "<<edge_string<<endl;
                            }
                        }else{
                            incrementEdgeSet.insert(edge_string);
                        }
                    }else{
                        string edge_string = std::to_string(u) + "\t" + std::to_string(v);
                        if(decreaseEdgeSet.find(edge_string)!=decreaseEdgeSet.end()){
                            if(incrementEdgeSet.find(edge_string)!=incrementEdgeSet.end()){
                                incrementEdgeSet.erase(edge_string);
                            }else{
                                cout<<"[ERROR] randance of edge deletion : "<<edge_string<<endl;
                            }
                        }else{
                            decreaseEdgeSet.insert(edge_string);
                        }
                    }
                }else{
                    cout << "[WARNING]: Out of range: i=" << i<<", j="<<j << endl;
                    break;
                }
            }
            //remove randanecy edges
            set<string> commonEdges = DataProcess::getCommonStringSet(decreaseEdgeSet,incrementEdgeSet);
            for(set<string>::iterator ceit=commonEdges.begin();ceit!=commonEdges.end();ceit++){
                decreaseEdgeSet.erase(*ceit);
                incrementEdgeSet.erase(*ceit);
            }

            int u,v;
            bool c;
            string edge_string;
            for(set<string>::iterator desit=decreaseEdgeSet.begin();desit!=decreaseEdgeSet.end();desit++){
                edge_string = *desit;
                DataProcess::decodeEdgeLine(edge_string,u,v,c);
                set<int> edge;
                edge.insert(u);
                edge.insert(v);
                decreaseEdges.push_back(edge);

                set<int> neighborSet_v;
                if(neighborMap.find(v)!=neighborMap.end()){
                    neighborSet_v = neighborMap[v];
                }
                neighborSet_v.erase(u);
                if(neighborSet_v.size()>0){
                    neighborMap[v] = neighborSet_v;
                }else{
                    neighborMap.erase(v);
                }

                set<int> neighborSet_u;
                if(neighborMap.find(u)!=neighborMap.end()){
                    neighborSet_u = neighborMap[u];
                }
                neighborSet_u.erase(v);
                if(neighborSet_u.size()>0){
                    neighborMap[u] = neighborSet_u;
                }else{
                    neighborMap.erase(u);
                }
            }

            map<int,set<int> > tempGraph = neighborMap;
            neighborMap = originGraph;
            originGraph = tempGraph;

            vector<set<int> > addCliques = batchInsertUpdate(decreaseEdges);
            vector<set<int> > deleteCliques = getDeleteCliques(addCliques,decreaseEdges,true,originGraph);
            updateCliqueResult(deleteCliques, addCliques);

            neighborMap = originGraph;

            for(set<string>::iterator iesit=incrementEdgeSet.begin();iesit!=incrementEdgeSet.end();iesit++){
                edge_string = *iesit;
                DataProcess::decodeEdgeLine(edge_string,u,v,c);
                set<int> edge;
                edge.insert(u);
                edge.insert(v);
                incrementEdges.push_back(edge);

                set<int> neighborSet_v;
                if(neighborMap.find(v)!=neighborMap.end()){
                    neighborSet_v = neighborMap[v];
                }
                neighborSet_v.insert(u);
                neighborMap[v] = neighborSet_v;

                set<int> neighborSet_u;
                if(neighborMap.find(u)!=neighborMap.end()){
                    neighborSet_u = neighborMap[u];
                }
                neighborSet_u.insert(v);
                neighborMap[u] = neighborSet_u;
            }

            vector<set<int> > addCliques2 = batchInsertUpdate(incrementEdges);
            vector<set<int> > deleteCliques2 = getDeleteCliques(addCliques2,incrementEdges,false,originGraph);
            updateCliqueResult(addCliques2, deleteCliques2);

            if(WRITE_BATCH_RESULT == true) {
                int cliqueNumber = recordResult(DATA_NAME1 + "-"+std::to_string(BATCH_SIZE)+ "-VLDBJ-" + std::to_string(i) + ".txt");
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


vector<set<int> > VLDBJ::batchInsertUpdate(list<set<int> > incrementEdges){
    vector<set<int> > addCliques;
    set<string> excludeEdges;

    for(list<set<int> >::iterator ieIt=incrementEdges.begin();ieIt!=incrementEdges.end();ieIt++){
        set<int> cand, fini, K = *ieIt;

        int u,v;
        if(K.size()==2){
            set<int>::iterator uit = K.begin();
            set<int>::reverse_iterator vit = K.rbegin();
            u = *uit;
            v = *vit;
        }else{
            cout<<"[ERROR3]: edge format error!"<<endl;
        }
        map<int,set<int> > subgraphNeighborMap = getCommonSubgraph(u,v,cand);

        tomitaExcludeEdges(addCliques,K,cand,fini,excludeEdges,subgraphNeighborMap);

        excludeEdges.insert(DataProcess::edgeToString(u,v));
    }

    return addCliques;
}


vector<set<int> > VLDBJ::getDeleteCliques(vector<set<int> > addCliques, list<set<int> > incrementEdges, bool deletion, map<int,set<int> > originGraph){
    vector<set<int> > deleteCliques;
    for(vector<set<int> >::iterator acit=addCliques.begin();acit!=addCliques.end();acit++){
        set<int> c= *acit;
        set<set<int> > S;
        S.insert(c);
        for(list<set<int> >::iterator ieit=incrementEdges.begin();ieit!=incrementEdges.end();ieit++){
            set<int> edge = *ieit;
            set<int>::iterator uit = edge.begin();
            set<int>::reverse_iterator vit = edge.rbegin();
            int u = *uit;
            int v = *vit;
            if(c.find(u)!=c.end() && c.find(v)!=c.end()){//e=(u,v)
                set<set<int> > S_hat;
                for(set<set<int> >::iterator shit=S.begin();shit!=S.end();shit++){
                    set<int> c_hat = *shit;
                    if(c_hat.find(u)!=c_hat.end() && c_hat.find(v)!=c_hat.end()){//e in E(c1)
                        set<int> c1 = c_hat;
                        c1.erase(u);
                        set<int> c2 = c_hat;
                        c2.erase(v);
                        S_hat.insert(c1);
                        S_hat.insert(c2);
                    }else{
                        S_hat.insert(c_hat);
                    }
                }
                S = S_hat;
            }
        }
        for(set<set<int> >::iterator sit=S.begin();sit!=S.end();sit++){
            set<int> c_hat = *sit;
            if(deletion==false){
                string c_string = DataProcess::cliqueToString(c_hat);
                if(cliqueResultIndex.find(c_string)!=cliqueResultIndex.end()){
                    deleteCliques.push_back(c_hat);
                }
            }else{//The VLDBJ paper has an error here, so we fix it via maximal validation.
                if(c_hat.size()>1 && isMaximal(c_hat,originGraph)==true){
                    deleteCliques.push_back(c_hat);
                }
            }
        }
    }

    return deleteCliques;
}


void VLDBJ::updateCliqueResult(vector<set<int> > addCliques, vector<set<int> > deleteCliques){
    for(vector<set<int> >::iterator acit=addCliques.begin();acit!=addCliques.end();acit++){
        set<int> addClique = *acit;
        string ac_string = DataProcess::cliqueToString(addClique);
        cliqueResultIndex.insert(ac_string);
    }

    for(vector<set<int> >::iterator dcit=deleteCliques.begin();dcit!=deleteCliques.end();dcit++){
        set<int> delClique = *dcit;
        string dc_string = DataProcess::cliqueToString(delClique);
        cliqueResultIndex.erase(dc_string);
    }
}

