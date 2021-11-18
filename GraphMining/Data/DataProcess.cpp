//
//  DataProcess.cpp
//  kECC
//
//  Created by Yuting on 18/8/27.
//  Copyright (c) 2018年 Yuting. All rights reserved.
//

#include "DataProcess.h"


void DataProcess::loadMaximalClique(const string &filePath, list<set<int> > &maximal_clique_list){
    ifstream dataFile(filePath.c_str());
    if(dataFile.is_open()) {
        string line;
        while (getline(dataFile, line)) {
            set<int> cliqueSet;
            vector<string> nodes;
            trim(line);
            split(line, " ", nodes);
            for(vector<string>::iterator nit=nodes.begin();nit!=nodes.end();nit++){
                string node = *nit;
                int nodeId;
                stringstream ss1;
                ss1 << node;
                ss1 >> nodeId; //string -> int
                cliqueSet.insert(nodeId);
            }
            maximal_clique_list.push_back(cliqueSet);
        }
        dataFile.close();
    }else{
        cout<<"Open File Error!"<<endl;
    }
}


void DataProcess::trim(string & str )
{
    string::size_type pos0 = str.find_last_not_of('\r');
    if (pos0 != string::npos)
    {
        str = str.substr(0, pos0 + 1);
    }

    string::size_type pos = str.find_first_not_of(' ');
    if (pos != string::npos)
    {
        string::size_type pos2 = str.find_last_not_of(' ');
        if (pos2 != string::npos)
        {
            str = str.substr(pos, pos2 - pos + 1);
        }else{
            str = str.substr(pos);
        }
    }
};


void DataProcess::split(const string& src, const string& separator, vector<string>& dest)
{
    string str = src;
    string substring;
    string::size_type start = 0, index;

    do{
        index = str.find_first_of(separator,start);
        if (index != string::npos)
        {
            substring = str.substr(start,index-start);
            dest.push_back(substring);
            start = str.find_first_not_of(separator,index);
            if (start == string::npos) return;
        }
    }while(index != string::npos);

    //the last token
    substring = str.substr(start);
    dest.push_back(substring);
};

string DataProcess::to_string(int num){
    stringstream ss;
    ss << num;
    string str = ss.str(); //string -> int

    return str;
}

int DataProcess::to_int(string str){
    int num;
    stringstream ss;
    ss << str;
    ss >> num;

    return num;
}

bool DataProcess::decodeEdgeLine(string str, int &u, int &v, bool &add){
    add = true;
    DataProcess::trim(str);
    if (str.at(0) == '#') {
        return false;
    }
    stringstream ss;
    vector<string> nodes;
    DataProcess::split(str, "	", nodes); // format: (-/+ )v u
    if (nodes.size() == 3) {
        if (nodes[0] == "-") {
            add = false;
        }
        ss.clear();
        ss << nodes[1];
        ss >> u; //string -> int
        ss.clear();
        ss << nodes[2];
        ss >> v; //string -> int
    } else if (nodes.size() == 2) {
        ss.clear();
        ss << nodes[0];
        ss >> u; //string -> int
        ss.clear();
        ss << nodes[1];
        ss >> v; //string -> int
    } else {
        cout << "Exception of data format: " << str << endl;
        return false;
    }
    //adjust the order of u v
    if(u>v){
        u-=v;
        v+=u;
        u=v-u;

        return false;
    }
    return true;
}



/**
 * raw date format:   v1	v2
 * clean data format: (+/-)	v1	v2
 * print statistics of the data.
 */
void DataProcess::preprocessRawData(const string &rawFilePath, const string &newFilePath){
    int edgeNumber = 0;
    set<string> insertEdgeMap, deleteEdgeMap;
    ifstream rawDataFile(rawFilePath.c_str());
    ofstream newDataFile(newFilePath.c_str());
    if(rawDataFile.is_open() && newDataFile.is_open()) {
        string line;
        while (getline(rawDataFile, line)) {
            vector<string> nodes;
            trim(line);
            if (line.at(0) != '#') {
                split(line, "\t", nodes);
                int u,v;
                if (nodes.size() == 2) {
                    u = to_int(nodes[0]);
                    v = to_int(nodes[1]);
                    if(u>v){
                        u += v;
                        v = u - v;
                        u -= v;
                    }
                    string newLine = std::to_string(u)+"\t"+std::to_string(v);
                    if(u!=v){
                        if(insertEdgeMap.find(newLine)==insertEdgeMap.end()){
                            newDataFile << newLine <<"\n";
                            edgeNumber++;
                            insertEdgeMap.insert(newLine);
                        }
                    }
                }else if (nodes.size() == 3){
                    u = to_int(nodes[1]);
                    v = to_int(nodes[2]);
                    if(u>v){
                        u += v;
                        v = u - v;
                        u -= v;
                    }
                    string newLine = std::to_string(u)+"\t"+std::to_string(v);
                    if(u!=v){
                        if(nodes[0]=="+"){
                            if(insertEdgeMap.find(newLine)==insertEdgeMap.end()){
                                newDataFile << nodes[0] << "\t" << newLine <<"\n";
                                edgeNumber++;
                                insertEdgeMap.insert(newLine);
                            }else{
                                cout<<newLine<<endl;
                            }
                        }else if (nodes[0]=="-") {
                            if (deleteEdgeMap.find(newLine) == deleteEdgeMap.end()) {
                                newDataFile << nodes[0] << "\t" << newLine << "\n";
                                edgeNumber++;
                                deleteEdgeMap.insert(newLine);
                            }else{
                                cout<<newLine<<endl;
                            }
                        }
                    }else{
                        cout<<newLine<<endl;
                    }
                }else{
                    continue;
                }
            }
        }
    }
    rawDataFile.close();
    newDataFile.close();
    cout<<"edgeNumber="<<edgeNumber<<endl;
}


void DataProcess::constructDecreasedRawData(const string &rawFilePath, const string &newFilePath){
    ifstream rawDataFile(rawFilePath.c_str());
    ofstream newDataFile(newFilePath.c_str());
    if(rawDataFile.is_open() && newDataFile.is_open()) {
        list<string> appendEdges;
        string line,newline;
        while (getline(rawDataFile, line)) {
            vector<string> nodes;
            trim(line);
            if (line.at(0) != '#') {
                int u,v;
                bool add;
                decodeEdgeLine(line,u,v,add);
                if(add==true){
                    newline = "-\t" + line;
                }else{
                    newline = to_string(u) + "\t" + to_string(v);
                }
                appendEdges.push_back(newline);
                newDataFile << line <<"\n";
            }
        }
        for(list<string>::iterator aeit=appendEdges.begin();aeit!=appendEdges.end();aeit++){
            string edge = *aeit;
            newDataFile << edge <<"\n";
        }
    }
    rawDataFile.close();
    newDataFile.close();
}

string DataProcess::compareResult(const string &filePath1, const string &filePath2){
    string result = "";
    string IdNOTinfile1 = "[Ids] NOT in file1 :";
    string IdNOTinfile2 = "[Ids] NOT in file2 :";
    ifstream dataFile1(filePath1.c_str());
    ifstream dataFile2(filePath2.c_str());
    if(dataFile1.is_open() && dataFile2.is_open()){
        bool same = true;
        string line1;
        set<string> record1;
        while(getline(dataFile1,line1)){
            trim(line1);
            if(record1.find(line1)!=record1.end()){
                cout<<"[WORNING1]: find same record in file1:"<<line1<<endl;
            }
            record1.insert(line1);
        }
        string line2;
        set<string> record2;
        while(getline(dataFile2,line2)){
            trim(line2);
            if(record2.find(line2)!=record2.end()){
                cout<<"[WORNING2]: find same record in file2:"<<line2<<endl;
            }
            record2.insert(line2);
            if(record1.find(line2)==record1.end()){
                cout<<"NOT in file1: "<<line2<<endl;
                same = false;
            }else{
                record1.erase(line2);
            }
        }

        for(set<string>::iterator rit = record1.begin();rit!=record1.end();rit++){
            cout<<"NOT in file2: "<<*rit<<endl;
            same = false;
        }

        if(same){
            cout<<"[CHECK] : ok!"<<endl;
            result ="[CHECK] : ok!\n";
        }else{
            result ="[CHECK] : error!\n";

        }
    }else{
        cout<<"OPEN file failed!"<<endl;
        result = "OPEN file failed!\n";
    }

    return result;
}


set<string> DataProcess::getCommonStringSet(const set<string> s1,const set<string> s2){
    set<string> des;
    des.clear();
    set<string>::iterator it1 = s1.begin();
    set<string>::iterator it2 = s2.begin();
    while(it1!=s1.end() && it2!=s2.end()){
        if(*it1 == *it2){
            des.insert(*it1);
            it1 ++;
            it2 ++;
        }else if(*it1 < *it2){
            it1 ++;
        }else{
            it2 ++;
        }
    }
    return des;
}

set<int> DataProcess::getCommonSet(const set<int> S1,const set<int> S2){
    set<int> des;
    des.clear();
    set<int>::iterator it1 = S1.begin();
    set<int>::iterator it2 = S2.begin();
    while(it1!=S1.end() && it2!=S2.end()){
        if(*it1 == *it2){
            des.insert(*it1);
            it1 ++;
            it2 ++;
        }else if(*it1 < *it2){
            it1 ++;
        }else{
            it2 ++;
        }
    }
    return des;
}

set<int> DataProcess::getDifferentSet(set<int> S1, set<int> S2) {
    set<int> S3 = S1;
    set<int>::iterator it1 = S1.begin();
    set<int>::iterator it2 = S2.begin();
    while (it1 != S1.end() && it2 != S2.end()) {
        if (*it1 == *it2) {
            S3.erase(*it1);
            it1++;
            it2++;
        } else if (*it1 < *it2) {
            it1++;
        } else {
            it2++;
        }
    }
    return S3;
}


bool DataProcess::isSubSet(set<int> smallSet, set<int> bigSet){
    if(smallSet.size()>bigSet.size()){
        return false;
    }
    set<int>::iterator it1 = smallSet.begin();
    set<int>::iterator it2 = bigSet.begin();
    while (it1 != smallSet.end() && it2 != bigSet.end()) {
        if (*it1 == *it2) {
            it1++;
            it2++;
        } else if (*it1 < *it2) {
            return false;
        } else {
            it2++;
        }
    }
    if(it1 != smallSet.end() ){
        return false;
    }

    return true;
}

bool DataProcess::isEqualSet(set<int> S1, set<int> S2){
    if(S1.size()!=S2.size()){
        return false;
    }
    set<int>::iterator it1 = S1.begin();
    set<int>::iterator it2 = S2.begin();
    while (it1 != S1.end() && it2 != S2.end()) {
        if (*it1 == *it2) {
            it1++;
            it2++;
        } else {
            return false;
        }
    }
    return true;
}

bool DataProcess::hasCommonElement(set<int> S1, set<int> S2){
    set<int>::iterator it1 = S1.begin();
    set<int>::iterator it2 = S2.begin();
    while (it1 != S1.end() && it2 != S2.end()) {
        if (*it1 == *it2) {
            return true;
        }else {
            if(*it1 < *it2){
                it1++;
            }else{
                it2++;
            }
        }
    }
    return false;
}

string DataProcess::cliqueToString(set<int> clique){
    string s = "";
    for(set<int>::iterator cit=clique.begin();cit!=clique.end();cit++){
        if(cit!=clique.begin()){
            s+=" ";
        }
        s+=std::to_string(*cit);
    }
    return s;
}

set<int> DataProcess::stringToClique(string c_string){
    set<int> c_set;
    vector<string> nodes;
    DataProcess::split(c_string," ",nodes);
    stringstream ss;
    int node_id;
    for(int i=0;i<nodes.size();i++){
        string node = nodes[i];
        ss.clear();
        ss<<node;
        ss>>node_id;
        c_set.insert(node_id);
    }
    return c_set;
}

string DataProcess::processTimeStamps(list<clock_t> timeStamps){
    stringstream ss;
    string result = "";
    clock_t ct1, ct2;
    double duration;
    for(list<clock_t>::iterator tsit=timeStamps.begin();tsit!=timeStamps.end();tsit++){
        if(tsit==timeStamps.begin()){
            ct1 = *tsit;
        }else{
            ct2 = *tsit;
            duration = (double)(ct2-ct1)/CLOCKS_PER_SEC;
            ss.clear();
            ss<<duration;
            string d;
            ss>>d;
            result += d + " ";
            ct1 = ct2;
        }
    }
    result += "\n";
    return result;
}


string DataProcess::edgeToString(int u, int v){
    //adjust the order of u v
    if(u>v){
        u-=v;
        v+=u;
        u=v-u;
    }

    stringstream ss;
    ss << u;
    string str = ss.str(); //string -> int
    ss.clear();
    ss << v;
    str+="\t"+ss.str();

    return str;
}
