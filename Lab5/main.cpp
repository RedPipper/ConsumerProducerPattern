//
// Created by tefan on 11/18/23.
//
#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <map>
#include "tsQueue.h"
#include "listaParticipanti.h"
#include <cstring>
#include <filesystem>
#include <atomic>

using namespace std;

string currentPath = "/home/tefan/Facultate/PPD/ConsumerProducerPattern/Lab5";
struct row{
    int part_id;
    int score;
};
mutex idMutex;
map<int, mutex> idLockMap;

void generate(){
    filesystem::create_directory(currentPath + "/data");
    srand(time(nullptr));
    for (int c = 1; c <= 5; c++){
        int parts_count = 80 + rand() % 20;
        for (int i = 1; i <= 10; i++)
        {
            ofstream f(currentPath + "/data/RezultateC" + to_string(c) + "_P" + to_string(i) + ".txt");
            for (int k = 1; k <= parts_count; k++)
            {
                int score = (unsigned)rand() % 100 == 8 ? -1 : rand() % 11;
                f << 100 * c + k << " " << score << "\n";
            }

            f.close();

        }
    }
}

void rulareSecventiala(){
    listaParticipanti parts_list;
    for (const auto& entry : filesystem::directory_iterator(currentPath + "/data"))
    {
        ifstream f(entry.path());
        for (int part, score; f >> part >> score; parts_list.add_score(part, score));
        f.close();
    }

    ofstream g(currentPath + "/result_secv.txt");
    g << parts_list;
    g.close();
}

void runConsumer(tsQueue<row>* prodQueue, listaParticipanti* listaParticipanti, int pid){
    int entries = 0;
    while(prodQueue->isActive() || prodQueue->getSize()){
        auto x = prodQueue->pop();
        if(x){
            idMutex.lock();
            lock_guard<mutex> lock(idLockMap[x->part_id]);
            idMutex.unlock();
            listaParticipanti->add_score(x->part_id, x->score);
            ++entries;
        }

    }
    printf(" \n");
}

void runProducer(tsQueue<row>* prodQueue, int pr, int pid){
    int q = 50 / pr, r = 50 % pr;
    int start = pid * q + (r <= pid ? r : pid);
    int end = (pid + 1) * q + (r <= (pid + 1) ? r : pid + 1);
    for (int k = start; k < end; k++){
        int country = k / 10 + 1, part = k % 10 + 1;
        ifstream f(currentPath + "/data/RezultateC" + to_string(country) + "_P" + to_string(part) + ".txt");

        for (int part, score, k = 0; k < 50000 && (f >> part >> score); k++){
            auto aux = new row{part, score};
            prodQueue->put(aux);
        }

        f.close();
    }
}

void rulareParalela(int pr, int pw){
    listaParticipanti listaParticipanti(true);
    tsQueue<row> prodQueue(100);
    thread* consumers = new thread[pw]{};
    for (int p = 0; p < pw; p++) consumers[p] = thread(runConsumer, &prodQueue, &listaParticipanti, p);


    thread* producers = new thread[pr]{};
    for (int p = 0; p < pr; p++) producers[p] = thread(runProducer, &prodQueue, pr, p);

    for (int p = 0; p < pr; p++) producers[p].join();

    prodQueue.stop();
    for (int p = 0; p < pw; p++) consumers[p].join();

    delete[] producers;

    ofstream g(currentPath + "/result_par.txt");
    g << listaParticipanti;
    g.close();
}


int main(int argc, char** argv){
//        generate();
    auto t_start = std::chrono::high_resolution_clock::now();
    if (argc > 1 && strcmp(argv[1], "secv") == 0)
    {
        rulareSecventiala();
    }
    else
    {
        int pw = argc > 2 ? atoi(argv[2]) : 16;
        int pr = argc > 3 ? atoi(argv[3]) : 1;
        rulareParalela(pr, pw);
    }
    auto t_end = std::chrono::high_resolution_clock::now();
    long elapsed_time_ms = std::chrono::duration_cast<chrono::microseconds>(t_end - t_start).count();
    printf("%ld\n", elapsed_time_ms);

    return 0;
}