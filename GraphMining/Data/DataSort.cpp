//
// Created by Lenovo on 2021/4/6.
//

#include "DataSort.h"

void DataSort::sort(){
    map<int,int> appearIndex,degreeIndex,degeneracyIndex;
    map<int,int> appearIndex_rev,degreeIndex_rev,degeneracyIndex_rev;
    int appearID=1,degreeID=1,degeneracyID=1;

    string loadPath = DATA_PATH + DATA_NAME + ".txt";
    ifstream readFile(loadPath.c_str());
    if(readFile.is_open()){
        map<int,set<int>> neighborMap;
        string line;
        int u,v;
        bool add = true;
        while(getline(readFile,line)){
            bool unexchange = DataProcess::decodeEdgeLine(line, u, v, add);
            if(unexchange==false){
                u-=v;
                v+=u;
                u=v-u;
            }
            if(add==true){
                set<int> u_neighbor,v_neighbor;
                if(neighborMap.find(u)!=neighborMap.end()){
                    u_neighbor = neighborMap[u];
                }
                u_neighbor.insert(v);
                neighborMap[u] = u_neighbor;

                if(neighborMap.find(v)!=neighborMap.end()){
                    v_neighbor = neighborMap[v];
                }
                v_neighbor.insert(u);
                neighborMap[v] = v_neighbor;

                //appear sort
                if(appearIndex.find(u)==appearIndex.end()){
                    appearIndex_rev[appearID] = u;
                    appearIndex[u] = appearID++;
                }
                if(appearIndex.find(v)==appearIndex.end()){
                    appearIndex_rev[appearID] = v;
                    appearIndex[v] = appearID++;
                }

            }else{
                cout<<"[ERROR]:"<<"read line format error!"<<endl;
            }
        }

        //degree sort
        map<int,set<int>> degreeGroupIndex;
        for(map<int,set<int>>::iterator nmit=neighborMap.begin();nmit!=neighborMap.end();nmit++){
            int vertex = (*nmit).first;
            set<int> neighbors = (*nmit).second;
            int degree = neighbors.size();
            set<int> vertexOfDegree;
            if(degreeGroupIndex.find(degree)!=degreeGroupIndex.end()){
                vertexOfDegree = degreeGroupIndex[degree];
            }
            vertexOfDegree.insert(vertex);
            degreeGroupIndex[degree] = vertexOfDegree;
        }
        for(map<int,set<int>>::iterator dgit=degreeGroupIndex.begin();dgit!=degreeGroupIndex.end();dgit++){
            int degree = (*dgit).first;
            set<int> vertices = (*dgit).second;
            for(set<int>::iterator vit=vertices.begin();vit!=vertices.end();vit++){
                int v = *vit;
                degreeIndex_rev[degreeID] = v;
                degreeIndex[v] = degreeID++;
            }
        }

        //degeneracy sort
        while(degreeGroupIndex.size()>0){
            map<int,set<int>>::iterator dgit=degreeGroupIndex.begin();
            int degree = (*dgit).first;
            set<int> vertices = (*dgit).second;
            set<int>::iterator vit=vertices.begin();
            int v = *vit;
            degeneracyIndex_rev[degeneracyID] = v;
            degeneracyIndex[v] = degeneracyID++;
            vertices.erase(v);
            if(vertices.size()>0){
                degreeGroupIndex[degree] = vertices;
            }else{
                degreeGroupIndex.erase(degree);
            }
            if(neighborMap.find(v)!=neighborMap.end()){
                set<int> neighbors = neighborMap[v];
                for(set<int>::iterator nit=neighbors.begin();nit!=neighbors.end();nit++){
                    int nei = *nit;
                    set<int> neiNeighbors = neighborMap[nei];
                    int neiDegree = neiNeighbors.size();
                    set<int> neiVertices = degreeGroupIndex[neiDegree];
                    neiVertices.erase(nei);
                    if(neiVertices.size()>0){
                        degreeGroupIndex[neiDegree] = neiVertices;
                    }else{
                        degreeGroupIndex.erase(neiDegree);
                    }
                    neiDegree--;
                    set<int> newNeiVertices;
                    if(degreeGroupIndex.find(neiDegree)!=degreeGroupIndex.end()){
                        newNeiVertices = degreeGroupIndex[neiDegree];
                    }
                    newNeiVertices.insert(nei);
                    degreeGroupIndex[neiDegree] = newNeiVertices;

                    neiNeighbors.erase(v);
                    if(neiNeighbors.size()>0){
                        neighborMap[nei] = neiNeighbors;
                    }else{
                        neighborMap.erase(nei);
                    }
                }
                neighborMap.erase(v);
            }
        }
        readFile.close();

    }else{
        cout<<"[ERROR]:"<<"open file failed!"<<endl;
    }

    //save
    string appearPath = DATA_PATH + DATA_NAME + "-appear.txt";
    string degreePath = DATA_PATH + DATA_NAME + "-degree.txt";
    string degeneracyPath = DATA_PATH + DATA_NAME + "-degeneracy.txt";
    ofstream appearDataFile(appearPath.c_str());
    ofstream degreeDataFile(degreePath.c_str());
    ofstream degeneracyDataFile(degeneracyPath.c_str());
    ifstream readFile2(loadPath.c_str());
    if(readFile2.is_open() && appearDataFile.is_open() && degreeDataFile.is_open() && degeneracyDataFile.is_open()){
        map<int,set<int>> neighborMap;
        string line,line2;
        int u,v,u_ID,v_ID;
        bool add = true;
        while(getline(readFile2,line)) {
            bool unexchange = DataProcess::decodeEdgeLine(line, u, v, add);
            if(unexchange==false){
                u-=v;
                v+=u;
                u=v-u;
            }
            if (add == true) {
                u_ID = appearIndex[u];
                v_ID = appearIndex[v];
                line2 = DataProcess::to_string(u_ID) + "	" + DataProcess::to_string(v_ID);
                appearDataFile << line2 <<"\n";

                u_ID = degreeIndex[u];
                v_ID = degreeIndex[v];
                line2 = DataProcess::to_string(u_ID) + "	" + DataProcess::to_string(v_ID);
                degreeDataFile << line2 <<"\n";

                u_ID = degeneracyIndex[u];
                v_ID = degeneracyIndex[v];
                line2 = DataProcess::to_string(u_ID) + "	" + DataProcess::to_string(v_ID);
                degeneracyDataFile << line2 <<"\n";
            }
        }
        readFile2.close();
        appearDataFile.close();
        degreeDataFile.close();
        degeneracyDataFile.close();
    }else{
        cout<<"[ERROR]:"<<"open file2 failed!"<<endl;
    }

    string appearIndexPath = DATA_PATH + DATA_NAME + "-index-appear.txt";
    string degreeIndexPath = DATA_PATH + DATA_NAME + "-index-degree.txt";
    string degeneracyIndexPath = DATA_PATH + DATA_NAME + "-index-degeneracy.txt";
    ofstream appearIndexFile(appearIndexPath.c_str());
    ofstream degreeIndexFile(degreeIndexPath.c_str());
    ofstream degeneracyIndexFile(degeneracyIndexPath.c_str());
    if(appearIndexFile.is_open() && degreeIndexFile.is_open() && degeneracyIndexFile.is_open()) {
        string line3;
        for( map<int,int>::iterator airIt=appearIndex_rev.begin();airIt!=appearIndex_rev.end();airIt++){
            line3 = DataProcess::to_string((*airIt).first) + "	" + DataProcess::to_string((*airIt).second);
            appearIndexFile << line3 << "\n";
        }
        appearIndexFile.close();
        for( map<int,int>::iterator dirIt=degreeIndex_rev.begin();dirIt!=degreeIndex_rev.end();dirIt++){
            line3 = DataProcess::to_string((*dirIt).first) + "	" + DataProcess::to_string((*dirIt).second);
            degreeIndexFile << line3 << "\n";
        }
        degreeIndexFile.close();
        for( map<int,int>::iterator dyirIt=degeneracyIndex_rev.begin();dyirIt!=degeneracyIndex_rev.end();dyirIt++){
            line3 = DataProcess::to_string((*dyirIt).first) + "	" + DataProcess::to_string((*dyirIt).second);
            degeneracyIndexFile << line3 << "\n";
        }
        degeneracyIndexFile.close();
    }else{
        cout<<"[ERROR]:"<<"open file3 failed!"<<endl;
    }

}



void DataSort::restore(){
    string targetAppendix; // 1:degeneracy; 2:degree; 3:appear;
    for(set<int>::iterator stit=SORT_TYPE.begin();stit!=SORT_TYPE.end();stit++){
        int sort= *stit;
        if(sort==1){
            targetAppendix = "-degeneracy";
        }else if(sort==2){
            targetAppendix = "-degree";
        }else if(sort==3){
            targetAppendix = "-appear";
        }

        string targetFileName = DATA_PATH + DATA_NAME + targetAppendix + ".txt";
        string indexFileName = DATA_PATH + DATA_NAME + "-index" + targetAppendix + ".txt";
        string restoreFileName = DATA_PATH + DATA_NAME + targetAppendix + "-restore.txt";
        ifstream targetFile(targetFileName.c_str());
        ifstream indexFile(indexFileName.c_str());
        ofstream restoreFile(restoreFileName.c_str());
        if(targetFile.is_open() && indexFile.is_open() && restoreFile.is_open()) {
            string line;
            map<int,int> indexMap;
            while(getline(indexFile,line)) {
                vector<string> nodes;
                DataProcess::split(line,"	",nodes);
                int id = DataProcess::to_int(nodes[0]);
                int vertex = DataProcess::to_int(nodes[1]);
                indexMap[id]=vertex;
            }

            int u,v;
            bool add;
            while(getline(targetFile,line)) {
                bool unexchange = DataProcess::decodeEdgeLine(line, u, v, add);
                if(unexchange==false){
                    u-=v;
                    v+=u;
                    u=v-u;
                }
                u = indexMap[u];
                v = indexMap[v];
                restoreFile << DataProcess::to_string(u) + "	" + DataProcess::to_string(v) << "\n";
            }

            targetFile.close();
            indexFile.close();
            restoreFile.close();
        } else{
            cout<<"[ERROR]:"<<"open file4 failed!"<<endl;
        }

    }
}