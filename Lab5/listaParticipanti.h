//
// Created by tefan on 12/7/23.
//

#ifndef LAB5_LISTAPARTICIPANTI_H
#define LAB5_LISTAPARTICIPANTI_H


#include <set>
#include <fstream>
#include <mutex>
#include <atomic>
using namespace std;

class listaParticipanti
{
private:
    struct node
    {
        int part_id, score = 0;
        node* next;
        mutex nodeLock;
    };
    node* head = new node{-1, -1, nullptr};
    node* tail = new node{-1,-1,head};
    static void lock(node* a){
        if(a)a->nodeLock.lock();
    }

    static void unlock(node* a){
        if(a)a->nodeLock.unlock();
    }

    static void insertAfter(node* n,node* target){
        node* aux = n->next;
        n->next = target;
        target->next = aux;
    }

    void insert(node* target){
        node* aux = tail;
        if(is_parallel){
            while(!aux->nodeLock.try_lock()){ // find an unlocked node
                aux = aux->next;
                if(aux == head)aux = tail;
            }
            lock(target);
            insertAfter(aux, target);
            unlock(aux);
            unlock(target);

        }else{
            insertAfter(tail, target);
        }

    }

    node* getNode(int part_id){
        node* aux = tail->next;
        while(aux != head){
            if(aux->part_id == part_id){
                return aux;
            }
            aux = aux->next;
        }
        return nullptr;
    }

    static node* createNewNode(int part_id){
        return new node{part_id, 0, nullptr };
    }

    static bool order(node* a, node* b){
        return a->score > b->score || (a->score == b->score && a->part_id <= b->part_id);
    }

    bool is_parallel;

public:
    explicit listaParticipanti(bool is_parallel=false) : is_parallel(is_parallel){ }

    void bubbleSort() {
        for (node* i = tail->next; i->next != head; i = i->next) {
            for (node* j = tail->next; j->next != head; j = j->next) {
                if (j->next && !order(j, j->next)) {
                    swap(j, j->next);
                }
            }
        }
    }

    void swap(node* a, node* b) {
        int tempPartId = a->part_id;
        int tempScore = a->score;

        a->part_id = b->part_id;
        a->score = b->score;

        b->part_id = tempPartId;
        b->score = tempScore;
    }


    void add_score(int participant, int score){
        node* partNode = getNode(participant);

        if(partNode == nullptr){
            partNode = createNewNode(participant);
            insert(partNode);
        }
        if(is_parallel) lock(partNode);
        if(partNode->score == -1){
            if(is_parallel) unlock(partNode);
            return;
        }
        if(score == -1)
            partNode->score = -1;
        else
            partNode->score += score;
        if(is_parallel) unlock(partNode);

    }

    friend ostream& operator << (ostream& outFile, listaParticipanti& pl)
    {
        pl.bubbleSort();
        for (node* n = pl.tail->next; n != pl.head ; n = n->next){
            if(n->score != -1){
                outFile << n->part_id << " " << n->score << "\n";
            }
        }
        outFile << "Descalificati:\n";
        for (node* n = pl.tail->next; n != pl.head ; n = n->next){
            if(n->score == -1){
                outFile << n->part_id << "\n";
            }
        }
        return outFile;
    }
};


#endif //LAB5_LISTAPARTICIPANTI_H
