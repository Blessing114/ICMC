//
// Created by Lenovo on 2020/12/28.
//

#include "ICDE.h"

list<clock_t>  ICDE::loadData(){
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
                        //sort u,v
                        int p,q;
                        if(neighbor_u.size()<=neighbor_v.size()){
                            p=u;
                            q=v;
                        }else{
                            p=v;
                            q=u;
                        }
                        insertEdge(p,q);
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
                }else{
                    cout << "[WARNING]: Out of range: i=" << i<<", j="<<j << endl;
                    break;
                }
            }

            if(WRITE_BATCH_RESULT == true) {
                int cliqueNumber = recordResult(
                        DATA_NAME1 + "-" + std::to_string(BATCH_SIZE) + "-ICDE-" + std::to_string(i) + ".txt");
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


void ICDE::insertEdge(int p, int q){
    set<string> deleteCliques, addCliques;
    for(set<string>::iterator it = cliqueResultIndex.begin();it!=cliqueResultIndex.end();it++){
        string C_string = *it;
        set<int> C = DataProcess::stringToClique(C_string);
        if(C.find(p)!=C.end()){
            deleteCliques.insert(C_string);
            set<int> neighbor_q = neighborMap[q];
            set<int> C_cand = DataProcess::getCommonSet(C,neighbor_q);
            C_cand.insert(q);
            set<int> C1 = C;
            C1.insert(q);
            if(DataProcess::isEqualSet(C_cand,C1)==true){
                addCliques.insert(DataProcess::cliqueToString(C_cand));
            }else{
                addCliques.insert(C_string);
                if(maxEval(C_cand)==true){
                    addCliques.insert(DataProcess::cliqueToString(C_cand));
                }
            }
        } else if(C.find(q)!=C.end()){
            deleteCliques.insert(C_string);
            set<int> neighbor_p = neighborMap[p];
            set<int> C_cand = DataProcess::getCommonSet(C,neighbor_p);
            C_cand.insert(p);
            set<int> C2 = C;
            C2.insert(p);
            if(DataProcess::isEqualSet(C_cand,C2)==false){
                addCliques.insert(C_string);
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


void ICDE::deleteEdge(int u, int v){
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


bool ICDE::maxEval(set<int> cand){
    if(cand.size()<1){
        return false;
    }
    int v_min=-1, deg_min=INT32_MAX;
    for(set<int>::iterator cit=cand.begin();cit!=cand.end();cit++){
        int v = *cit;
        set<int> neighbor_v = neighborMap[v];
        if(deg_min>neighbor_v.size()){
            v_min = v;
            deg_min = neighbor_v.size();
        }
    }

    set<int> neighbor_vmin = neighborMap[v_min];
    set<int> N =  DataProcess::getDifferentSet(neighbor_vmin, cand);
    for(set<int>::iterator nit=N.begin();nit!=N.end();nit++){
        int v = *nit;
        set<int> neighbor_v = neighborMap[v];
        if(neighbor_v.size()>=cand.size()){
            if(DataProcess::isSubSet(cand,neighbor_v)==true){
                return false;
            }
        }
    }

    return true;
}