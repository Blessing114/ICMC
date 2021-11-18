#include <time.h>
#include <sys/time.h>
#include<ctime>

#include "Data/DataSort.h"
#include "Data/DataProcess.h"
#include "MCE/ICMC.h"
#include "MCE/VLDBJ.h"
#include "MCE/ICDE.h"
#include "MCE/PGM.h"
#include "MCE/COA.h"

using namespace std;

clock_t alg_start, alg_end;
double alg_duration;

string int_to_string(int num){
    stringstream ss;
    ss << num;
    string str = ss.str(); //string -> int

    return str;
}

vector<int> generateCursorVector(int edgeNumber, int batchSize, bool doubleSize){
    vector<int> cursorVector;
    int totalNumber = 0;
    while(edgeNumber>totalNumber){
        int remain = edgeNumber-totalNumber;
        int newBatch = batchSize<remain?batchSize:remain;
        cursorVector.push_back(newBatch);
        totalNumber+=newBatch;
    }
    if(doubleSize==true){
        int num = cursorVector.size();
        for(int i=num-1;i>=0;i--){
            int temp = cursorVector[i];
            cursorVector.push_back(temp);
        }
    }
    return cursorVector;
}


string compareResult(const string &filePath1, const string &filePath2){
    string result = "";
    ifstream dataFile1(filePath1.c_str());
    ifstream dataFile2(filePath2.c_str());
    if(dataFile1.is_open() && dataFile2.is_open()){
        bool same = true;
        string line1;
        set<string> record1;
        while(getline(dataFile1,line1)){
            //trim(line1);
            if(record1.find(line1)!=record1.end()){
                cout<<"[WORNING1]: find same record in file1:"<<line1<<endl;
            }
            record1.insert(line1);
        }
        string line2;
        set<string> record2;
        while(getline(dataFile2,line2)){
            //trim(line2);
            if(record2.find(line2)!=record2.end()){
                cout<<"[WORNING2]: find same record in file2:"<<line2<<endl;
            }
            record2.insert(line2);
            if(record1.find(line2)==record1.end()){
                //cout<<"NOT in file1: "<<line2<<endl;
                same = false;
            }else{
                record1.erase(line2);
            }
        }

        for(set<string>::iterator rit = record1.begin();rit!=record1.end();rit++){
            //cout<<"NOT in file2: "<<*rit<<endl;
            same = false;
        }

        if(same){
            cout<<"[CHECK] : ok!"<<endl;
            result ="[CHECK] : ok!\n";
        }else{
            cout<<"[CHECK] : error!"<<endl;
            result ="[CHECK] : error!\n";
        }
    }else{
        cout<<"OPEN result file failed!"<<endl;
        result = "OPEN result file failed!\n";
    }

    return result;
}


int main(int argc, char **argv)
{
    cout<<"start!! \n"<<endl;
    string dataPath = "../max_clique/"; //Here is the graph data path, modify it according to your environment

    struct tm* current_time;
    const time_t t0 = time(NULL);
    current_time = localtime(&t0);
    cout<<"\n\n =====test time : "<<1900 + current_time->tm_year<<"-"<<1 + current_time->tm_mon<<"-"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<"======\n";
    ofstream logFile;
    string LOG_FILE_PATH = dataPath + "log-c.txt";
    logFile.open(LOG_FILE_PATH.c_str(),ios::app);
    logFile<<"\n\n =====test time : "<<1900 + current_time->tm_year<<"-"<<1 + current_time->tm_mon<<"-"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<"======\n";

    map<string,int> dataList;
    
    dataList.insert(make_pair("SSCA_13_58776_200212_mix",200212)); //edge number:
    dataList.insert(make_pair("PLG_100000_150435_150435_513070_mix",513070)); //edge number:
    dataList.insert(make_pair("PLG_50000_73578_73578_250816_mix",250816)); //edge number:
    dataList.insert(make_pair("ER_50000_125436_125436_427772_mix",427772)); //edge number:
    //添加数据

    list<int> batchSizeList;

    // batchSizeList.push_back(100);
    // batchSizeList.push_back(200);
    // batchSizeList.push_back(500);
    batchSizeList.push_back(1000);
    batchSizeList.push_back(2000);
    batchSizeList.push_back(5000);
    batchSizeList.push_back(7000);
    batchSizeList.push_back(10000);
    batchSizeList.push_back(20000);
    //添加batch size

    list<clock_t> timeStamps;
    string time_stamp_string;
    for(map<string,int>::iterator dit = dataList.begin();dit!=dataList.end();dit++){
        string dataName = (*dit).first;
        int edgeNumber = (*dit).second;
        bool doubleSize = false;
        if(dataName.find("_add_minus")!=string::npos){
            doubleSize = true;
        }

        /** sort clean data **/
        //DataSort *ds = new DataSort(dataName,dataPath+"clean/",0);
        //ds->sort();
        //ds->restore();
        //delete ds;
        /** sort raw data **/


        //DataProcess::preprocessRawData(dataPath+"raw/"+dataName+".txt",dataPath+"clean/"+dataName+".txt");
        //DataProcess::constructDecreasedRawData(dataPath+"clean/"+dataName+".txt",dataPath+"clean/"+dataName+"-ID.txt");
        for(list<int>::iterator bslit=batchSizeList.begin();bslit!=batchSizeList.end();bslit++){
            int batchSize = *bslit;

            /** To execute the entire processing **/
            // batchSize = edgeNumber;
            /** To execute the entire processing **/

            string batch_size_string = int_to_string(batchSize);

            cout<<"\n"<<"[dataName]="<<dataName<<"  [batchSize]="<<batch_size_string<<endl;
            logFile<<"\n"<<"[dataName]="<<dataName<<"  [batchSize]="<<batch_size_string<<endl;

            vector<int> cursorVector = generateCursorVector(edgeNumber,batchSize,doubleSize);
            //vector<int> cursorVector = {10000,10000,10000,10000,10000,11251,10000,10000,10000,10000,10000,10000,10000};

            alg_start = clock();
            ICMC* incremCMC = new ICMC(dataPath, dataName, cursorVector);
            incremCMC->setBatchSize(batchSize);
            timeStamps = incremCMC->loadData();
            delete incremCMC;
            alg_end = clock();
            alg_duration = (double)(alg_end-alg_start)/CLOCKS_PER_SEC;
            cout<<"  IncremCMC --> Total = "<<std::fixed<<alg_duration<<endl;
            logFile<<"  IncremCMC --> Total = "<<std::fixed<<alg_duration<<endl;
            time_stamp_string = DataProcess::processTimeStamps(timeStamps);
            logFile<<time_stamp_string<<endl;

            alg_start = clock();
            ICMC* incremCMC_Opt1 = new ICMC(dataPath, dataName, cursorVector);
            incremCMC_Opt1->setBatchSize(batchSize);
            timeStamps = incremCMC_Opt1->loadData_Opt1();
            delete incremCMC_Opt1;
            alg_end = clock();
            alg_duration = (double)(alg_end-alg_start)/CLOCKS_PER_SEC;
            cout<<"  IncremCMC_Opt1 --> Total = "<<std::fixed<<alg_duration<<endl;
            logFile<<"  IncremCMC_Opt1 --> Total = "<<std::fixed<<alg_duration<<endl;
            time_stamp_string = DataProcess::processTimeStamps(timeStamps);
            logFile<<time_stamp_string<<endl;


            alg_start = clock();
            ICDE* idce = new ICDE(dataPath, dataName, cursorVector);
            idce->setBatchSize(batchSize);
            timeStamps = idce->loadData();
            delete idce;
            alg_end = clock();
            alg_duration = (double)(alg_end-alg_start)/CLOCKS_PER_SEC;
            cout<<"  ICDE --> Total = "<<std::fixed<<alg_duration<<endl;
            logFile<<"  ICDE --> Total = "<<std::fixed<<alg_duration<<endl;
            time_stamp_string = DataProcess::processTimeStamps(timeStamps);
            logFile<<time_stamp_string<<endl;



            alg_start = clock();
            VLDBJ* vldbj = new VLDBJ(dataPath, dataName, cursorVector);
            vldbj->setBatchSize(batchSize);
            timeStamps = vldbj->loadData();
            delete vldbj;
            alg_end = clock();
            alg_duration = (double)(alg_end-alg_start)/CLOCKS_PER_SEC;
            cout<<"  VLDBJ --> Total = "<<std::fixed<<alg_duration<<endl;
            logFile<<"  VLDBJ --> Total = "<<std::fixed<<alg_duration<<endl;
            time_stamp_string = DataProcess::processTimeStamps(timeStamps);
            logFile<<time_stamp_string<<endl;

            alg_start = clock();
            PGM* pgm = new PGM(dataPath, dataName, cursorVector);
            pgm->setBatchSize(batchSize);
            timeStamps = pgm->loadData();
            delete pgm;
            alg_end = clock();
            alg_duration = (double)(alg_end-alg_start)/CLOCKS_PER_SEC;
            cout<<"  PGM --> Total = "<<std::fixed<<alg_duration<<endl;
            logFile<<"  PGM --> Total = "<<std::fixed<<alg_duration<<endl;
            time_stamp_string = DataProcess::processTimeStamps(timeStamps);
            logFile<<time_stamp_string<<endl;

/*
            alg_start = clock();
            COA* coa = new COA(dataPath, dataName, cursorVector);
            coa->setBatchSize(batchSize);
            timeStamps = coa->loadData();
            delete coa;
            alg_end = clock();
            alg_duration = (double)(alg_end-alg_start)/CLOCKS_PER_SEC;
            cout<<"  COA --> Total = "<<std::fixed<<alg_duration<<endl;
            logFile<<"  COA --> Total = "<<std::fixed<<alg_duration<<endl;
            time_stamp_string = DataProcess::processTimeStamps(timeStamps);
            logFile<<time_stamp_string<<endl;
*/



            if(WRITE_BATCH_RESULT == true){
                for(int i=0;i<cursorVector.size();i++){
                    string i_string = int_to_string(i);
                    cout<<"Batch "<<i_string<<" : ";
                    compareResult(dataPath+"result/"+dataName+"-"+batch_size_string+"-IncremCMC-"+ i_string +".txt",dataPath+"result/"+dataName+"-"+batch_size_string+"-IncremCMCOPt1-" + i_string +".txt");
                    //compareResult(dataPath+"result/"+dataName+"-"+batch_size_string+"-IncremCMC-"+ i_string +".txt",dataPath+"result/"+dataName+"-"+batch_size_string+"-IncremCMC2-" + i_string +".txt");
                    //compareResult(dataPath+"result/"+dataName+"-"+batch_size_string+"-IncremCMC-"+ i_string +".txt",dataPath+"result/"+dataName+"-"+batch_size_string+"-VLDBJ-" + i_string +".txt");
                    //compareResult(dataPath+"result/"+dataName+"-"+batch_size_string+"-incremCMC-"+ i_string +".txt",dataPath+"result/"+dataName+"-"+batch_size_string+"-ICDE-" + i_string +".txt");
                    //compareResult(dataPath+"result/"+dataName+"-"+batch_size_string+"-incremCMC-"+ i_string +".txt",dataPath+"result/"+dataName+"-"+batch_size_string+"-PGM-" + i_string +".txt");
                    //compareResult(dataPath+"result/"+dataName+"-"+batch_size_string+"-incremCMC-"+ i_string +".txt",dataPath+"result/"+dataName+"-"+batch_size_string+"-COA-" + i_string +".txt");
                }
            }

        }
    }
    cout<<"\n end!!"<<endl;
}