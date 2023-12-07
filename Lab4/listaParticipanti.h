//
// Created by tefan on 12/7/23.
//

#ifndef LAB4_LISTAPARTICIPANTI_H
#define LAB4_LISTAPARTICIPANTI_H


#include <set>
#include <fstream>
#include <mutex>

using namespace std;

class listaParticipanti
{
private:
    struct node
    {
        int part_id, score = 0;
        node* next;
    };
    node* head = nullptr;

    node* get(int participant)
    {
        node* aux = head, *before = nullptr;
        while (aux != nullptr){
            if (aux->part_id == participant){
                if (before != nullptr)
                    before->next = aux->next;
                else
                    head = aux->next;
                aux->next = nullptr;
                return aux;

            }
            before = aux, aux = aux->next;

        }

        return new node{participant, 0, nullptr };
    }

    bool order(node* a, node* b){
        return a->score > b->score || (a->score == b->score && a->part_id <= b->part_id);

    }

    void put(node* target){
        if (head == nullptr){
            head = target;
            return;

        }
        if (order(target, head)){
            target->next = head;
            head = target;
            return;

        }

        node* aux = head;
        while (aux->next != nullptr){
            if (order(aux, target) && order(target, aux->next)){
                target->next = aux->next;
                aux->next = target;
                return;

            }
            aux = aux->next;

        }

        aux->next = target;
    }

    set<int> disqualified;
    mutex listMutex;

    bool is_parallel;

public:
    listaParticipanti(bool is_parallel=false) : is_parallel(is_parallel) { }

    void add_score(int participant, int score){
        if(is_parallel) listMutex.lock();
        ([&](){
            if (disqualified.find(participant) != disqualified.end()) return;
            node* n = get(participant);
            if (score < 0){
                delete n;
                disqualified.insert(participant);
                return;

            }
            n->score += score;
            put(n);
        }
        )();
        if(is_parallel) listMutex.unlock();
    }

    friend ostream& operator << (ostream& outFile, const listaParticipanti& pl)
    {
        for (node* n = pl.head; n; n = n->next)
            outFile << n->part_id << " " << n->score << "\n";
        outFile << "Descalificati:\n";

        for (auto d : pl.disqualified) outFile << d << "\n";
        return outFile;
    }
};


#endif //LAB4_LISTAPARTICIPANTI_H
