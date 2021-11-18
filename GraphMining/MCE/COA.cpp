//
// Created by Lenovo on 2021/1/3.
//

#include "COA.h"

list<clock_t>  COA::loadData(){
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
                    set<int> neighbor_u, neighbor_v;
                    if(neighborMap.find(u)!=neighborMap.end()){
                        neighbor_u = neighborMap[u];
                    }else{
                        string uString = DataProcess::to_string(u);
                        cliqueResultIndex.insert(uString);
                    }
                    if(neighborMap.find(v)!=neighborMap.end()){
                        neighbor_v = neighborMap[v];
                    }else{
                        string vString = DataProcess::to_string(v);
                        cliqueResultIndex.insert(vString);
                    }
                    if(add==true){
                        neighbor_u.insert(v);
                        neighbor_v.insert(u);
                        neighborMap[u] = neighbor_u;
                        neighborMap[v] = neighbor_v;
                        insertEdge(u,v);
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

                }else {
                    cout << "[WARNING]: Out of range: i=" << i << ", j=" << j << endl;
                    break;
                }
            }

            if(WRITE_BATCH_RESULT == true) {
                int cliqueNumber = recordResult(
                        DATA_NAME1 + "-" + std::to_string(BATCH_SIZE) + "-COA-" + std::to_string(i) + ".txt");
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


void COA::insertEdge(int u, int v){
    set<string> deleteCliques, addCliques;
    list<set<int> > MC_u,MC_v,K;
    for(set<string>::iterator it = cliqueResultIndex.begin();it!=cliqueResultIndex.end();it++){
        string C_string = *it;
        set<int> C = DataProcess::stringToClique(C_string);
        if(C.find(u)!=C.end()) {
            MC_u.push_back(C);
            if(maxEval(C)==false){
                deleteCliques.insert(C_string);
            }
        }
        if(C.find(v)!=C.end()) {
            MC_v.push_back(C);
            if(maxEval(C)==false){
                deleteCliques.insert(C_string);
            }
        }
    }

    for(list<set<int> >::iterator uit=MC_u.begin();uit!=MC_u.end();uit++){
        set<int> Cu = *uit;
        for(list<set<int> >::iterator vit=MC_v.begin();vit!=MC_v.end();vit++){
            set<int> Cv = *vit;
            set<int> common = DataProcess::getCommonSet(Cu,Cv);
            common.insert(u);
            common.insert(v);
            if(maxEval(common) == true){
                addCliques.insert(DataProcess::cliqueToString(common));
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


void COA::deleteEdge(int u, int v){
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


bool COA::maxEval(set<int> C){
    for(map<int,set<int> >::iterator nmit=neighborMap.begin();nmit!=neighborMap.end();nmit++){
        int v = (*nmit).first;
        if(C.find(v)==C.end()){
            set<int> neighbors = neighborMap[v];
            if(DataProcess::isSubSet(C,neighbors)== true){
                return false;
            }
        }
    }
    return true;
}