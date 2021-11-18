//
// Created by Lenovo on 2020/11/25.
//

#include "ICMC.h"

/**
 * Load graph data from an edge file, format: (-/+ )v u
 * @param filePath
 * @param cursorVector: record the batch incremental pattern of the data
 */
list<clock_t> ICMC::loadData(){
    list<clock_t> timeStamps;
    string filePath = FILE_PATH1 + "clean/" + DATA_NAME1 + ".txt";
    ifstream dataFile(filePath.c_str());
    if(dataFile.is_open()){
        ofstream recordFile;
        set<string> cliqueResultIndex_last_time;
        if(RECORD_BATCH_DIFFERENCE == true){//preprocessing to record batch_difference of each dataset
            string recordFilePath = FILE_PATH1 + "result/" + DATA_NAME1 + "-batch_difference-" + std::to_string(BATCH_SIZE) + ".txt";
            recordFile.open(recordFilePath.c_str());
            if (recordFile.is_open()== false){
                cout<<"[ERROR]: open record file failed."<<endl;
            }
            recordFile<<"changeCliqueNumber,changeVertexNumber,changeEdgeNumber\n";
        }

        clock_t ts;
        string line;
        int u,v;
        bool add = true;
        for(int i=0;i<CURSOR_VECTOR.size();i++) {
            ts = clock();
            timeStamps.push_back(ts);
            int cursor = CURSOR_VECTOR[i];
            set<string> deleteEdgeList;
            map<int, int> biggestNeighborIndex;//index for each big vertex v to record the biggest neighbor u of it.
            for (int j = 0; j < cursor; j++) {
                if (getline(dataFile, line)) {
                    DataProcess::decodeEdgeLine(line, u, v, add);
                    string edge_string = DataProcess::to_string(u) + "\t" + DataProcess::to_string(v);
                    if (add == true) {
                        if (deleteEdgeList.find(edge_string) == deleteEdgeList.end()) {
                            set<int> smallNeighborSet_v;
                            if (smallNeighborMap.find(v) != smallNeighborMap.end()) {
                                smallNeighborSet_v = smallNeighborMap[v];
                            }
                            smallNeighborSet_v.insert(u);
                            smallNeighborMap[v] = smallNeighborSet_v;

                            set<int> bigNeighborSet_u;
                            if (bigNeighborMap.find(u) != bigNeighborMap.end()) {
                                bigNeighborSet_u = bigNeighborMap[u];
                            }
                            bigNeighborSet_u.insert(v);
                            bigNeighborMap[u] = bigNeighborSet_u;

                            if (biggestNeighborIndex.find(v) != biggestNeighborIndex.end()) {
                                if (biggestNeighborIndex[v] < u) {
                                    biggestNeighborIndex[v] = u;
                                }
                            } else {
                                biggestNeighborIndex[v] = u;
                            }
                        } else {
                            deleteEdgeList.erase(edge_string);
                        }

                    } else {
                        deleteEdgeList.insert(edge_string); //delete later

                        if (biggestNeighborIndex.find(v) != biggestNeighborIndex.end()) {
                            if (biggestNeighborIndex[v] < u) {
                                biggestNeighborIndex[v] = u;
                            }
                        } else {
                            biggestNeighborIndex[v] = u;
                        }
                    }

                } else {
                    cout << "[WARNING] Out of range: i=" << i << ", j=" << j << endl;
                    break;
                }
            }

            //translate to smallestNeighborIndex
            map<int, int> smallestNeighborIndex;
            for (map<int, int>::iterator bnit = biggestNeighborIndex.begin();
                 bnit != biggestNeighborIndex.end(); bnit++) {
                int v = (*bnit).first;
                int maxNeighbor = (*bnit).second;
                set<int> smallNeighborSet_v = smallNeighborMap[v];
                for (set<int>::iterator snit = smallNeighborSet_v.begin(); snit != smallNeighborSet_v.end(); snit++) {
                    int u = *snit;
                    if (u > maxNeighbor) {
                        break;
                    }
                    if (smallestNeighborIndex.find(u) == smallestNeighborIndex.end()) {
                        smallestNeighborIndex[u] = v;
                    }
                }
            }

            //delete edges in deleteEdgeList;
            for (set<string>::iterator delit = deleteEdgeList.begin(); delit != deleteEdgeList.end(); delit++) {
                string edge = *delit;
                DataProcess::decodeEdgeLine(edge, u, v, add);

                set<int> smallNeighborSet_v;
                if (smallNeighborMap.find(v) != smallNeighborMap.end()) {
                    smallNeighborSet_v = smallNeighborMap[v];
                }
                smallNeighborSet_v.erase(u);
                if (smallNeighborSet_v.size() > 0) {
                    smallNeighborMap[v] = smallNeighborSet_v;
                } else {
                    smallNeighborMap.erase(v);
                }

                set<int> bigNeighborSet_u;
                if (bigNeighborMap.find(u) != bigNeighborMap.end()) {
                    bigNeighborSet_u = bigNeighborMap[u];
                }
                bigNeighborSet_u.erase(v);
                if (bigNeighborSet_u.size() > 0) {
                    bigNeighborMap[u] = bigNeighborSet_u;
                } else {
                    bigNeighborMap.erase(u);
                }
            }

            //incremental execution
            for (map<int, int>::iterator snit = smallestNeighborIndex.begin();
                 snit != smallestNeighborIndex.end(); snit++) {
                int u = (*snit).first;
                int minNeighbor = (*snit).second;
                updateCandidate(u, minNeighbor);
            }

            if (DUBUG_SWITCH == true) {
                cout << "At End : ";
                printICT(root, "root");
                cout << endl;
            }

            if(RECORD_BATCH_DIFFERENCE == true) {// record the batch difference
                int changeCliqueNumber = 0, changeVertexNumber = 0, changeEdgeNumber = 0;
                getResultDifference(cliqueResultIndex_last_time, cliqueResultIndex, changeCliqueNumber,
                                    changeVertexNumber, changeEdgeNumber);

                string batchDifference =
                        "[" + DataProcess::to_string(i) + "]\t" + DataProcess::to_string(changeCliqueNumber) + "," +
                        DataProcess::to_string(changeVertexNumber) + "," + DataProcess::to_string(changeEdgeNumber) +
                        "\n";
                recordFile << batchDifference;
                cliqueResultIndex_last_time = cliqueResultIndex;
            }

            if(WRITE_BATCH_RESULT == true){
                int cliqueNumber = recordResult(DATA_NAME1 + "-" + std::to_string(BATCH_SIZE) + "-IncremCMC-" + std::to_string(i) + ".txt");
            }
        }

        ts = clock();
        timeStamps.push_back(ts);
        dataFile.close();

        if(RECORD_BATCH_DIFFERENCE == true) {// preprocessing to record batch_difference of each dataset
            recordFile.close();
        }
    }else{
        cout<<"Open File Error!"<<endl;
    }

    return timeStamps;
}

/**
 * Execute with optimal strategy1:
 * An incremental edge (u,v), for each {x|x<u, x \in N(v)}, purning the x that not connected with u.
 * */
list<clock_t> ICMC::loadData_Opt1(){
    list<clock_t> timeStamps;
    string filePath = FILE_PATH1 + "clean/" + DATA_NAME1 + ".txt";
    ifstream dataFile(filePath.c_str());
    if(dataFile.is_open()){
        ofstream recordFile;
        set<string> cliqueResultIndex_last_time;
        if(RECORD_BATCH_DIFFERENCE == true){//preprocessing to record batch_difference of each dataset
            string recordFilePath = FILE_PATH1 + "result/" + DATA_NAME1 + "-batch_difference-" + std::to_string(BATCH_SIZE) + ".txt";
            recordFile.open(recordFilePath.c_str());
            if (recordFile.is_open()== false){
                cout<<"[ERROR]: open record file failed."<<endl;
            }
            recordFile<<"changeCliqueNumber,changeVertexNumber,changeEdgeNumber\n";
        }

        clock_t ts;
        string line;
        int u,v;
        bool add = true;
        for(int i=0;i<CURSOR_VECTOR.size();i++) {
            ts = clock();
            timeStamps.push_back(ts);
            int cursor = CURSOR_VECTOR[i];
            set<string> deleteEdgeList;
            map<int, int> biggestNeighborIndex;//index for each big vertex v to record the biggest small neighbor u of it.
            map<int,set<int>> biggestNeighborSetMap;//record the incremental small neighbors of each big vertex v, optimal strategy1.
            for (int j = 0; j < cursor; j++) {
                if (getline(dataFile, line)) {
                    DataProcess::decodeEdgeLine(line, u, v, add);
                    string edge_string = DataProcess::to_string(u) + "\t" + DataProcess::to_string(v);
                    if (add == true) {
                        if (deleteEdgeList.find(edge_string) == deleteEdgeList.end()) {
                            set<int> smallNeighborSet_v;
                            if (smallNeighborMap.find(v) != smallNeighborMap.end()) {
                                smallNeighborSet_v = smallNeighborMap[v];
                            }
                            smallNeighborSet_v.insert(u);
                            smallNeighborMap[v] = smallNeighborSet_v;

                            set<int> bigNeighborSet_u;
                            if (bigNeighborMap.find(u) != bigNeighborMap.end()) {
                                bigNeighborSet_u = bigNeighborMap[u];
                            }
                            bigNeighborSet_u.insert(v);
                            bigNeighborMap[u] = bigNeighborSet_u;

                            set<int> biggestNeighborSet;
                            if (biggestNeighborIndex.find(v) != biggestNeighborIndex.end()) {
                                biggestNeighborSet = biggestNeighborSetMap[v];
                                if (biggestNeighborIndex[v] < u) {
                                    biggestNeighborIndex[v] = u;
                                }
                            } else {
                                biggestNeighborIndex[v] = u;
                            }
                            biggestNeighborSet.insert(u);
                            biggestNeighborSetMap[v]=biggestNeighborSet;

                        } else {
                            deleteEdgeList.erase(edge_string);
                        }
                    } else {
                        deleteEdgeList.insert(edge_string); //delete later

                        set<int> biggestNeighborSet;
                        if (biggestNeighborIndex.find(v) != biggestNeighborIndex.end()) {
                            biggestNeighborSet = biggestNeighborSetMap[v];
                            if (biggestNeighborIndex[v] < u) {
                                biggestNeighborIndex[v] = u;
                            }
                        } else {
                            biggestNeighborIndex[v] = u;
                        }
                        biggestNeighborSet.insert(u);
                        biggestNeighborSetMap[v]=biggestNeighborSet;
                    }

                } else {
                    cout << "[WARNING] Out of range: i=" << i << ", j=" << j << endl;
                    break;
                }
            }

            //translate to smallestNeighborIndex
            map<int, int> smallestNeighborIndex;
            for (map<int, int>::iterator bnit = biggestNeighborIndex.begin();bnit != biggestNeighborIndex.end(); bnit++) {
                int v = (*bnit).first;
                int maxNeighbor = (*bnit).second;
                set<int> incrementalElementSet = biggestNeighborSetMap[v];
                set<int> smallNeighborSet_v = smallNeighborMap[v];
                for (set<int>::iterator snit = smallNeighborSet_v.begin(); snit != smallNeighborSet_v.end(); snit++) {
                    int u = *snit;
                    if (u > maxNeighbor) {
                        break;
                    }
                    if (smallestNeighborIndex.find(u) == smallestNeighborIndex.end()) {
                        set<int> bigNeighbors_u = bigNeighborMap[u];
                        bigNeighbors_u.insert(u);
                        if(DataProcess::hasCommonElement(incrementalElementSet,bigNeighbors_u)==true){
                            smallestNeighborIndex[u] = v;
                        }
                    }
                }
            }

            //delete edges in deleteEdgeList;
            for (set<string>::iterator delit = deleteEdgeList.begin(); delit != deleteEdgeList.end(); delit++) {
                string edge = *delit;
                DataProcess::decodeEdgeLine(edge, u, v, add);

                set<int> smallNeighborSet_v;
                if (smallNeighborMap.find(v) != smallNeighborMap.end()) {
                    smallNeighborSet_v = smallNeighborMap[v];
                }
                smallNeighborSet_v.erase(u);
                if (smallNeighborSet_v.size() > 0) {
                    smallNeighborMap[v] = smallNeighborSet_v;
                } else {
                    smallNeighborMap.erase(v);
                }

                set<int> bigNeighborSet_u;
                if (bigNeighborMap.find(u) != bigNeighborMap.end()) {
                    bigNeighborSet_u = bigNeighborMap[u];
                }
                bigNeighborSet_u.erase(v);
                if (bigNeighborSet_u.size() > 0) {
                    bigNeighborMap[u] = bigNeighborSet_u;
                } else {
                    bigNeighborMap.erase(u);
                }
            }

            //incremental execution
            for (map<int, int>::iterator snit = smallestNeighborIndex.begin();
                 snit != smallestNeighborIndex.end(); snit++) {
                int u = (*snit).first;
                int minNeighbor = (*snit).second;
                updateCandidate(u, minNeighbor);
            }

            if (DUBUG_SWITCH == true) {
                cout << "At End : ";
                printICT(root, "root");
                cout << endl;
            }

            if(RECORD_BATCH_DIFFERENCE == true) {// record the batch difference
                int changeCliqueNumber = 0, changeVertexNumber = 0, changeEdgeNumber = 0;
                getResultDifference(cliqueResultIndex_last_time, cliqueResultIndex, changeCliqueNumber,
                                    changeVertexNumber, changeEdgeNumber);

                string batchDifference =
                        "[" + DataProcess::to_string(i) + "]\t" + DataProcess::to_string(changeCliqueNumber) + "," +
                        DataProcess::to_string(changeVertexNumber) + "," + DataProcess::to_string(changeEdgeNumber) +
                        "\n";
                recordFile << batchDifference;
                cliqueResultIndex_last_time = cliqueResultIndex;
            }

            if(WRITE_BATCH_RESULT == true){
                int cliqueNumber = recordResult(DATA_NAME1 + "-" + std::to_string(BATCH_SIZE) + "-IncremCMCOPt1-" + std::to_string(i) + ".txt");
            }
        }

        ts = clock();
        timeStamps.push_back(ts);
        dataFile.close();

        if(RECORD_BATCH_DIFFERENCE == true) {// preprocessing to record batch_difference of each dataset
            recordFile.close();
        }
    }else{
        cout<<"Open File Error!"<<endl;
    }

    return timeStamps;
}


/**
 * Update the candidate clique of u, with the minimal updated vertex v.
 * @param u: clique head, small vertex
 * @param minNeighbor: the min vertex in the updated neighbors
 */
void ICMC::updateCandidate(int u, int minNeighbor){
    if(DUBUG_SWITCH == true) {
        cout << "updateCandidate: " << u << " " << minNeighbor << endl;
    }
    // batch process  ======>
    map<int,list<CandidateCliqueBody* > > newCandidateBodyMap;
    if(candidateMap.find(u)!=candidateMap.end()) {//preprocessing candidate map
        map<int,list<CandidateCliqueBody* > > oldCandidateBodyMap = candidateMap[u];
        for(map<int,list<CandidateCliqueBody* > >::iterator cbmit=oldCandidateBodyMap.begin();cbmit!=oldCandidateBodyMap.end();cbmit++){
            int v = (*cbmit).first;
            list<CandidateCliqueBody* > candidateBodyList = (*cbmit).second;
            if(v<minNeighbor){
                for(list<CandidateCliqueBody* >::iterator cblit=candidateBodyList.begin();cblit!=candidateBodyList.end();cblit++){
                    CandidateCliqueBody* ccb = *cblit;
                    vector<int> cliqueBody = ccb->body;
                    InvertTreeNode* currentNode = ccb->treeNode;
                    removeCandidateClique(currentNode,cliqueBody,u);
                    vector<int> newCliqueBody;
                    for(int i=0;i<cliqueBody.size();i++){
                        if(cliqueBody[i]>=minNeighbor){
                            break;
                        }
                        newCliqueBody.push_back(cliqueBody[i]);
                    }
                    if(newCliqueBody.size()==0){
                        cout<<"[ERROR2]:"<<endl;
                    }
                    ccb->treeNode = NULL;
                    ccb->body = newCliqueBody;
                }
                newCandidateBodyMap[v] = candidateBodyList;
            }else{//clear bigger neighbors
                for(list<CandidateCliqueBody* >::iterator cblit=candidateBodyList.begin();cblit!=candidateBodyList.end();cblit++){
                    CandidateCliqueBody* ccb = *cblit;
                    vector<int> cliqueBody = ccb->body;
                    InvertTreeNode* currentNode = ccb->treeNode;
                    removeCandidateClique(currentNode,cliqueBody,u);
                    delete ccb;
                }
            }
        }
    }

    //update u-[minNeighbor,max]
    if(bigNeighborMap.find(u)!=bigNeighborMap.end()){
        set<int> bigNeighborSet_u = bigNeighborMap[u];
        for(set<int>::iterator bnit = bigNeighborSet_u.begin();bnit!=bigNeighborSet_u.end();bnit++){
            int v = *bnit;
            if(v<minNeighbor){
                continue;
            }
            //update u-v
            list<CandidateCliqueBody* > cliqueBodyListConstructAtV;
            if(newCandidateBodyMap.size()==0){
                CandidateCliqueBody* ccb = new CandidateCliqueBody;
                vector<int> newCliqueBody;
                newCliqueBody.push_back(v);
                ccb->treeNode = NULL;
                ccb->body = newCliqueBody;
                cliqueBodyListConstructAtV.push_back(ccb);
            }else{
                set<int> smallNeighborSet_v = smallNeighborMap[v];
                set<int>::iterator bit = bigNeighborSet_u.begin();
                set<int>::iterator sit = smallNeighborSet_v.begin();
                vector<int> commonNeighbor;
                while(bit!=bigNeighborSet_u.end() && sit!=smallNeighborSet_v.end()){
                    if(*bit == *sit){
                        commonNeighbor.push_back(*bit);
                        bit++;
                        sit++;
                    }else if(*bit < *sit){
                        bit++;
                    }else{
                        sit++;
                    }
                }
                if(commonNeighbor.size()==0){
                    CandidateCliqueBody* ccb = new CandidateCliqueBody;
                    vector<int> newCliqueBody;
                    newCliqueBody.push_back(v);
                    ccb->treeNode = NULL;
                    ccb->body = newCliqueBody;
                    cliqueBodyListConstructAtV.push_back(ccb);
                    newCandidateBodyMap[v] = cliqueBodyListConstructAtV;
                }else{
                    bool exit = false;
                    list<vector<int> > fullMatchList;
                    map<int,list<vector<int> > > partMatchList;
                    for(map<int,list<CandidateCliqueBody* > >::iterator ncbmit=newCandidateBodyMap.begin();ncbmit!=newCandidateBodyMap.end();ncbmit++){
                        list<CandidateCliqueBody*> candidateCliqueBodyList = (*ncbmit).second;
                        for(list<CandidateCliqueBody*>::iterator ccblit=candidateCliqueBodyList.begin();ccblit!=candidateCliqueBodyList.end();ccblit++){
                            CandidateCliqueBody* ccb = *ccblit;
                            vector<int> candidateBody = ccb->body;
                            vector<int> intersection;
                            int result = compare(commonNeighbor,candidateBody,intersection);
                            //@return 0:a==c==b   1:a==c=/=b   2:a=/=c==b   3:a=/=c=/=b
                            if(result==0){
                                candidateBody.push_back(v);
                                ccb->body = candidateBody;
                                exit = true;
                                break;
                            }else if(result==1){
                                CandidateCliqueBody* ccb = new CandidateCliqueBody;
                                vector<int> newCliqueBody;
                                intersection.push_back(v);
                                ccb->treeNode = NULL;
                                ccb->body = intersection;
                                cliqueBodyListConstructAtV.push_back(ccb);
                                exit = true;
                                break;
                            }else if(result==2){
                                candidateBody.push_back(v);
                                ccb->body = candidateBody;
                                fullMatchList.push_back(candidateBody);
                            }else{
                                intersection.push_back(v);
                                int size = intersection.size();
                                list<vector<int> > partMatchListOfSize;
                                if(partMatchList.find(size)!=partMatchList.end()){
                                    partMatchListOfSize = partMatchList[size];
                                }
                                partMatchListOfSize.push_back(intersection);
                                partMatchList[size] = partMatchListOfSize;
                            }
                        }
                        if(exit){
                            break;
                        }
                    }
                    if(exit==false && partMatchList.size()>0){
                        for(map<int,list<vector<int> > >::iterator pmlit=partMatchList.begin();pmlit!=partMatchList.end();pmlit++){
                            list<vector<int> > partMatchListOfSize = (*pmlit).second;
                            while(!partMatchListOfSize.empty()) {
                                vector<int> firstPartVector = partMatchListOfSize.back();;
                                partMatchListOfSize.pop_back();
                                bool partContain = false;
                                for(list<vector<int> >::iterator partIt = partMatchListOfSize.begin();partIt!=partMatchListOfSize.end();partIt++){
                                    vector<int> partVector =  *partIt;
                                    vector<int> temp;
                                    int result = compare(firstPartVector,partVector,temp);
                                    //@return 0:a==c==b   1:a==c=/=b   2:a=/=c==b   3:a=/=c=/=b
                                    if(result<=1){
                                        partContain = true;
                                        break;
                                    }
                                }
                                if(partContain!=true){
                                    map<int,list<vector<int> > >::iterator pmlit1=pmlit;
                                    for(pmlit1++;pmlit1!=partMatchList.end();pmlit1++) {
                                        list<vector<int> > partMatchListOfSize1 = (*pmlit1).second;
                                        for(list<vector<int> >::iterator partIt1 = partMatchListOfSize1.begin();partIt1!=partMatchListOfSize1.end();partIt1++){
                                            vector<int> partVector1 =  *partIt1;
                                            vector<int> temp;
                                            int result = compare(firstPartVector,partVector1,temp);
                                            //@return 0:a==c==b   1:a==c=/=b   2:a=/=c==b   3:a=/=c=/=b
                                            if(result<=1){
                                                partContain = true;
                                                break;
                                            }
                                        }
                                        if(partContain==true){
                                            break;
                                        }
                                    }
                                }
                                if(partContain!=true){
                                    bool fullContain = false;
                                    for(list<vector<int> >::iterator fullIt = fullMatchList.begin();fullIt!=fullMatchList.end();fullIt++){
                                        vector<int> fullVector =  *fullIt;
                                        vector<int> temp;
                                        int result = compare(firstPartVector,fullVector,temp);
                                        //@return 0:a==c==b   1:a==c=/=b   2:a=/=c==b   3:a=/=c=/=b
                                        if(result<=1){
                                            fullContain = true;
                                            break;
                                        }
                                    }
                                    if(fullContain!=true) {
                                        CandidateCliqueBody* ccb = new CandidateCliqueBody;
                                        ccb->treeNode = NULL;
                                        ccb->body = firstPartVector;
                                        cliqueBodyListConstructAtV.push_back(ccb);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if(cliqueBodyListConstructAtV.size()!=0){
                newCandidateBodyMap[v] = cliqueBodyListConstructAtV;
            }
        }
    }


    if(newCandidateBodyMap.size()>0){
        //iterate the newCandidateBodyMap to construct ICT
        for(map<int,list<CandidateCliqueBody* > >::iterator ncbmit=newCandidateBodyMap.begin();ncbmit!=newCandidateBodyMap.end();ncbmit++){
            list<CandidateCliqueBody* > cliqueBodies = (*ncbmit).second;
            for(list<CandidateCliqueBody* >::iterator cbit=cliqueBodies.begin();cbit!=cliqueBodies.end();cbit++){
                CandidateCliqueBody* cliqueBody = *cbit;
                if(cliqueBody->treeNode == NULL){
                    cliqueBody->treeNode = insertCandidateClique(cliqueBody->body,u);
                }
            }
        }
        candidateMap[u] = newCandidateBodyMap;
    }else{
        candidateMap.erase(u);
    }
}

/**
 * Calculate the intersection of two vectors.
 * @param a
 * @param b
 * @param c: the intersection vector of a and b
 * @return 0:a==c==b   1:a==c=/=b   2:a=/=c==b   3:a=/=c=/=b
 */
int ICMC::compare(vector<int> a, vector<int> b, vector<int> &c){
    vector<int>::iterator ait = a.begin();
    vector<int>::iterator bit = b.begin();
    while(ait!=a.end() && bit!=b.end()){
        if(*ait == *bit){
            c.push_back(*ait);
            ait++;
            bit++;
        }else if(*ait < *bit){
            ait++;
        }else{
            bit++;
        }
    }
    if(c.size()==a.size() && c.size()==b.size()){
        return 0;
    }else if(c.size()==a.size() && c.size()!=b.size()){
        return 1;
    }else if(c.size()!=a.size() && c.size()==b.size()){
        return 2;
    }else{
        return 3;
    }
}

/**
 * Update the InverseCliqueTree according to an inserted candidate clique
 * @param cliqueBody
 * @param cliqueHead
 * @return the TreeNode of cliqueHead
 */
InvertTreeNode* ICMC::insertCandidateClique(vector<int> cliqueBody, int cliqueHead){
    //***********************
    if(DUBUG_SWITCH == true) {
        cout << "Bofore [insert] : ";
        printICT(root, "root");
        cout << endl;
    }
    //***********************

    InvertTreeNode* currentNode = root;
    string clique_temp = "";
    for(int i = (int)cliqueBody.size()-1; i>=0;i--){
        int currentNodeName = cliqueBody[i];
        if(clique_temp!=""){
            clique_temp = std::to_string(currentNodeName) + " " + clique_temp;
        }else{
            clique_temp = std::to_string(currentNodeName);
        }
        map<int,InvertTreeNode*>::iterator it = currentNode->children.find(currentNodeName);
        if(it!=currentNode->children.end()){
            currentNode = (*it).second;
            if(currentNode->isMaximal == true){
                currentNode->isMaximal = false;
                cliqueResultIndex.erase(clique_temp);
            }
        }else{
            InvertTreeNode* newNode = new InvertTreeNode();
            newNode->parent = currentNode;
            currentNode->children.insert(make_pair(currentNodeName, newNode));
            currentNode = newNode;
        }
    }
    if(currentNode->children.find(cliqueHead)==currentNode->children.end()){
        InvertTreeNode* newNode = new InvertTreeNode(true);
        currentNode->children.insert(make_pair(cliqueHead, newNode));
        newNode->parent = currentNode;
        newNode->isMaximal = true;

        string clique_string = cliqueHeadBodyToString(cliqueHead, cliqueBody);
        cliqueResultIndex.insert(clique_string);

        //***********************
        if(DUBUG_SWITCH == true) {
            cout << "After  [insert] : ";
            printICT(root, "root");
            cout << endl;
        }
        //***********************

        return newNode;
    }else{
        map<int,InvertTreeNode*> children = currentNode->children;
        InvertTreeNode* newNode = children[cliqueHead];
        newNode->isLeaf = true;

        //***********************
        if(DUBUG_SWITCH == true) {
            cout << "After  [insert] : ";
            printICT(root, "root");
            cout << endl;
        } //***********************

        return newNode;
    }


}

/**
 * Update the InverseCliqueTree according to a removed candidate clique
 * @param tailNode: the TreeNode of the cliqueHead
 * @param cliqueBody
 * @param cliqueHead
 */
void ICMC::removeCandidateClique(InvertTreeNode* tailNode, vector<int> cliqueBody, int cliqueHead){
    //***********************
    if(DUBUG_SWITCH == true) {
        cout << "Bofore [remove] : ";
        printICT(root, "root");
        cout << endl;
    }//***********************

    string clique_String = cliqueHeadBodyToString(cliqueHead,cliqueBody);
    if(cliqueResultIndex.find(clique_String)!=cliqueResultIndex.end()){
        cliqueResultIndex.erase(clique_String);
    }

    if(tailNode->children.size()>0){
        tailNode->isLeaf = false;
        tailNode->isMaximal = false;
    }else{
        InvertTreeNode* pointNode = tailNode->parent;
        pointNode->children.erase(cliqueHead);
        delete tailNode;
        tailNode = pointNode;
        pointNode = tailNode->parent;
        for(vector<int>::iterator cbit=cliqueBody.begin();cbit!=cliqueBody.end();cbit++){
            cliqueHead = *cbit;
            if(tailNode->children.size()>0){
                break;
            }else if(tailNode->isLeaf == true){

                string add_clique = "";
                for(cbit;cbit!=cliqueBody.end();cbit++){
                    int node = *cbit;
                    if(add_clique!=""){
                        add_clique += " ";
                    }
                    add_clique += DataProcess::to_string(node);
                }
                cliqueResultIndex.insert(add_clique);
                tailNode->isMaximal = true;

                break;
            }
            pointNode->children.erase(cliqueHead);
            delete tailNode;
            tailNode = pointNode;
            pointNode = tailNode->parent;
        }
    }

    //***********************
    if(DUBUG_SWITCH == true) {
        cout << "After  [remove] : ";
        printICT(root, "root");
        cout << endl;
    }//***********************

}


int ICMC::recordResult2(string suffix){
    cliqueResultIndex2.clear();
    collectCliquefromICT(root, "");

    int cliqueNumber = 0;
    ofstream cliqueFile;
    string filePath = FILE_PATH1 + "result/" + suffix;
    cliqueFile.open(filePath.c_str());
    if (cliqueFile.is_open()){
        cliqueNumber = (int)cliqueResultIndex2.size();
        for(set<string>::iterator it = cliqueResultIndex2.begin();it!=cliqueResultIndex2.end();it++){
            cliqueFile<<*it<<"\n";
        }
        cliqueFile.close();
    }

    return cliqueNumber;
}