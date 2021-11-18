//
// Created by Lenovo on 2020/11/25.
//

#ifndef GRAPHMINING_ICMC_H
#define GRAPHMINING_ICMC_H

#include <bitset>
#include <stdio.h>
#include "../Data/DataProcess.h"
#include "IncrementalMCE.h"


using namespace std;

class InvertTreeNode
{
public:
    InvertTreeNode(){
        parent = NULL;
        isLeaf = false;
        isMaximal = false;
    };

    InvertTreeNode(bool leaf){
        parent = NULL;
        isLeaf = leaf;
        isMaximal = false;
    };

    InvertTreeNode(InvertTreeNode* parent, bool leaf){
        parent = parent;
        isLeaf = leaf;
        isMaximal = false;
    };

    InvertTreeNode(InvertTreeNode* parent, bool leaf, bool maximal){
        parent = parent;
        isLeaf = leaf;
        isMaximal = maximal;
    };

    InvertTreeNode* parent;
    map<int,InvertTreeNode*> children;
    bool isLeaf, isMaximal;
};

struct CandidateCliqueBody{
    vector<int> body;
    InvertTreeNode* treeNode;
};


class ICMC  : public IncrementalMCE {
public:
    ICMC(string TEST_FILE_PATH, string TEST_DATA_NAME, vector<int> cursorVector)
    : IncrementalMCE( TEST_FILE_PATH,TEST_DATA_NAME,cursorVector)
    {};

    ~ICMC(){
        destroy(root);
    };

    list<clock_t> loadData();

    list<clock_t> loadData_Opt1();

    void updateCandidate(int u,int minNeighbor);

    int compare(vector<int> a, vector<int> b, vector<int> &c);

    InvertTreeNode* insertCandidateClique(vector<int> cliqueBody, int cliqueHead);

    void removeCandidateClique(InvertTreeNode* tailNode, vector<int> cliqueBody, int cliqueHead);

    int recordResult2(string suffix);

private:
    set<string> cliqueResultIndex2;
    map<int, map<int,list<CandidateCliqueBody* > > > candidateMap;
    InvertTreeNode* root = new InvertTreeNode(NULL,true);

    void destroy(InvertTreeNode* itn){
        for(map<int,InvertTreeNode*>::iterator it=itn->children.begin();it!=itn->children.end();it++){
            destroy((*it).second);
        }
        delete itn;
    }

    string cliqueHeadBodyToString(int cliqueHead,vector<int> cliqueBody){
        string c_string = std::to_string(cliqueHead);
        for(vector<int>::iterator cit=cliqueBody.begin();cit!=cliqueBody.end();cit++){
            c_string += " ";
            c_string += std::to_string(*cit);
        }
        return c_string;
    }

    void printICT(InvertTreeNode* node, string name ){
        string leaf = "(0";
        if(node->isLeaf == true){
            leaf = "(1";
        }
        if(node->isMaximal == true){
            leaf += "1)";
        }else{
            leaf += "0)";
        }
        cout<<name<<leaf<<"{";
        map<int,InvertTreeNode*> childNodes = node->children;
        for(map<int,InvertTreeNode*>::iterator cnit=childNodes.begin();cnit!=childNodes.end();cnit++){
            if(cnit!=childNodes.begin()){
                cout<<", ";
            }
            int id = (*cnit).first;
            InvertTreeNode* childNode = (*cnit).second;
            printICT(childNode,std::to_string(id));
        }
        cout<<"}";
    };

    void collectCliquefromICT(InvertTreeNode* node, string suffix ){
        if(node->children.empty() && suffix!=""){
            cliqueResultIndex2.insert(suffix);
        }else{
            for(map<int,InvertTreeNode*>::iterator childIt = node->children.begin();childIt!=node->children.end();childIt++){
                int childName = (*childIt).first;
                InvertTreeNode* childNode = (*childIt).second;
                string child_suffix = std::to_string(childName);
                if(suffix!=""){
                    child_suffix = child_suffix + " " + suffix;
                }
                collectCliquefromICT(childNode,child_suffix);
            }
        }
    };

};


#endif //GRAPHMINING_ICMC_H
