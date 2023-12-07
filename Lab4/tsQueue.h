//
// Created by tefan on 12/7/23.
//

#ifndef CONSUMERPRODUCERPATTERN_TSQUEUE_H
#define CONSUMERPRODUCERPATTERN_TSQUEUE_H

#include <mutex>
#include <exception>
using namespace std;

template <typename T>
class tsQueue {
private:
    struct node{
        const T* data;
        node* before;

        node& operator=(const node& other)
        {
            if(other != this){
                this->data = other.data;
                this->before = other.before;
            }

            return *this;
        }
    };
    node* TAIL;
    node* HEAD;
    int maxSize = 0;
    int nrOfElems = 0;
    mutex blockMutex;

    bool pushWrapper(const T* object){
        if(!isFull()){
            ++nrOfElems;
            if(nrOfElems == 1){
                HEAD = new node{object, nullptr};
                TAIL = HEAD;
            }else{
                node* aux = new node{object, nullptr};
                HEAD->before = aux;
                HEAD = aux;
            }
            return true;
        }
        return false;
    }

    const T* getWrapper(){
        if(!isEmpty()){
            --nrOfElems;
            const T* aux = TAIL->data;
            node* aNode = TAIL;
            TAIL = TAIL->before;

            delete aNode;
            return aux;
        }
        return nullptr;
    }


public:

    bool isEmpty(){
        return nrOfElems == 0;
    }

    bool isFull(){
        return nrOfElems == maxSize;
    }

    explicit tsQueue(int maxSize){
        this->maxSize = maxSize;
    }


    void push(const T* data){
        blockMutex.lock();
        pushWrapper(data);
        blockMutex.unlock();
    }

    const T* get(){
        blockMutex.lock();
        const T* to_ret = getWrapper();
        blockMutex.unlock();

        return to_ret;
    }


};


#endif //CONSUMERPRODUCERPATTERN_TSQUEUE_H
