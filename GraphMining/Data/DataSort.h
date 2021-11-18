//
// Created by Lenovo on 2021/4/6.
//

#ifndef GRAPHMINING_DATASORT_H
#define GRAPHMINING_DATASORT_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include "DataProcess.h"

using namespace std;

class DataSort {
public:
    DataSort(string dataName, string dataPath, int sortType){
        DATA_NAME = dataName;
        DATA_PATH = dataPath;
        set<int> typeSet;
        if(sortType==0){
            typeSet.insert(1);
            typeSet.insert(2);
            typeSet.insert(3);
        }else{
            typeSet.insert(sortType);
        }
        SORT_TYPE = typeSet;
    }

    void sort();

    void restore();

private:
    string DATA_NAME;
    string DATA_PATH;
    set<int> SORT_TYPE;//0:save all type; 1:degeneracy; 2:degree; 3:appear;
};


#endif //GRAPHMINING_DATASORT_H
