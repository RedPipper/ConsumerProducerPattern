//
// Created by tefan on 11/18/23.
//

#ifndef CONSUMERPRODUCERPATTERN_TSQUEUE_H
#define CONSUMERPRODUCERPATTERN_TSQUEUE_H
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <atomic>

using namespace std;

template <typename T>
class tsQueue {
private:
    struct node{
        T* data;
        node* next;
    };

    int capacity;
    atomic<int> size;
    atomic<int> activeReaders = 0;
    bool active = true;
    node* tail;
    node* head;
    mutable mutex qMutex;
    condition_variable qEmpty_;
    condition_variable qFull_;

public:
    explicit tsQueue(int capacity) : capacity(capacity), size(0), tail(nullptr), head(nullptr){}

    void putAtomic(T* item){
        if(size == 0){
            head = tail = new node{item, nullptr};
        }else{
            head->next = new node{item, nullptr};
            head = head->next;
        }
        ++size;
    }


    void put(T* item){
        unique_lock<mutex> lock(qMutex);
        qFull_.wait(lock, [&]{return size.load() < capacity;});
        putAtomic(item);

        qEmpty_.notify_one();
    }

    T* popAtomic(){
        if(size == 0)return nullptr;
        T* aux = tail->data;
        node* an = tail;
        if(tail)tail = tail->next;

        delete an;
        --size;

        return aux;
    }

    T* pop(){
        unique_lock<mutex> lock(qMutex);
        ++activeReaders;
        qEmpty_.wait(lock, [&]{return !active || size.load() > 0;});


        T* aux = popAtomic();
        --activeReaders;

        qFull_.notify_one();

        return aux;
    }

    bool isActive(){
        return active;
    }

    int getSize(){
        return size.load();
    }

    void stop(){
        unique_lock<mutex> lock(qMutex);
        active = false;
        lock.unlock();

        while(activeReaders.load() > 0){
            lock.lock();
            qEmpty_.notify_all();
            lock.unlock();
        }
    }

};



#endif //CONSUMERPRODUCERPATTERN_TSQUEUE_H
