//
// Created by tefan on 11/18/23.
//
#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <map>
#include "tsQueue.h"
#include "InputParser.h"
using namespace std;
tsQueue< pair<int, int>> Q(50);
map<int, int> concMap;
mutex m_concMap;
int reads[200];


void writeToQueue(int thread_id){
    for(int i=thread_id * 100;i<(thread_id+1)*100;++i){
        auto aux = new pair<int, int>(thread_id, i);
        Q.push(aux);
    }
}

void readFromQueue(int thread_id){
    for(int i=0;i<100;++i){
        auto aux = Q.get();
        reads[i + 100*(thread_id - 1)] = aux->second;
    }
}

void readerThread(const string& filePath){
    ifstream fin(filePath);
    int C_ID, PUNCTAJ;
    while(fin>>C_ID>>PUNCTAJ){
        Q.push(new pair<int, int>(C_ID, PUNCTAJ));
    }
}

void workerThread(){

}

int main(int argc, char** argv){
    auto parser = new InputParser(argc, argv);

    if(!parser->cmdOptionExists("-p")){
        cout<<"Please insert the number of threads\n";
        exit(1);
    }
    if(!parser->cmdOptionExists("-f")){
        cout<<"Please insert the file to get the files from\n";
        exit(1);
    }
    if(!parser->cmdOptionExists("-o")){
        cout<<"Please insert the output file\n";
        exit(1);
    }


    int p = stoi(parser->getCmdOption("-p"));
    string files = parser->getCmdOption("-f");
    string outputFile = parser->getCmdOption("-o");

    vector<string> list;
    ifstream fin(files);

    string aux;
    while(fin>>aux){
        list.push_back(aux);
    }
    fin.close();
    int p_r = (int)list.size();
    int p_w = p - p_r;
    thread threads[p];
    for(int i=0; i<p_r;++i){
        threads[i] = thread(readerThread, list[i]);
    }
    for(int i=0;i<p_w;++i){
        threads[p_r + i] = thread(workerThread);
    }

    for(int i = 0; i < p; ++i){
        threads[i].join();
    }

    return 0;
}