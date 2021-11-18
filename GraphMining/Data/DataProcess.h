//
//  DataProcess.h
//  kECC
//
//  Created by Yuting on 18/8/27.
//  Copyright (c) 2018年 Yuting. All rights reserved.
//

#ifndef __kECC__DataProcess__
#define __kECC__DataProcess__

#include <time.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <math.h>

using namespace std;

class DataProcess{
public:

    DataProcess(const string &dataPath, const string &dataName){
        DATA_PATH = dataPath;
        DATA_NAME = dataName;
    };

    static void loadMaximalClique(const string &filePath, list<set<int> > &maximal_clique_list);

    static void trim(string & str);

    static void split(const string& src, const string& separator, vector<string>& dest);

    static string to_string(int num);

    static int to_int(string str);

    static bool decodeEdgeLine(string str, int &u, int &v, bool &add);

    static void preprocessRawData(const string &rawFilePath, const string &newFilePath);

    static void constructDecreasedRawData(const string &rawFilePath, const string &newFilePath);

    static string compareResult(const string &filePath1, const string &filePath2);

    static set<string> getCommonStringSet(const set<string> S1,const set<string> S2);

    static set<int> getCommonSet(const set<int> s1,const set<int> s2);

    static set<int> getDifferentSet(set<int> S1, set<int> S2);

    static bool isSubSet(set<int> smallSet, set<int> bigSet);

    static bool isEqualSet(set<int> S1, set<int> S2);

    static bool hasCommonElement(set<int> S1, set<int> S2);

    static string cliqueToString(set<int> clique);

    static set<int> stringToClique(string c_string);

    static string processTimeStamps(list<clock_t> timeStamps);

    static string edgeToString(int u, int v);

private:
    string DATA_PATH, DATA_NAME;

};

#endif /* defined(__kECC__DataProcess__) */
