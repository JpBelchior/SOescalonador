#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <string>
#include <bits/stdc++.h>

using namespace std;
enum Prioridade{
    ALTA,
    MEDIA,
    BAIXA,
    ES,
    PRONTO
};


struct Processo{
    int id;
    int tempoBurst;
    int numeroES;
    int ESAtual;
    int tempoRestante;
    Prioridade prioridade;
    bool frenteFilaES;
    int tempoprasair;

    Processo(int id, int tempoBurst, int numeroES){
        this->id = id;
        this->tempoBurst = tempoBurst;
        this->numeroES = numeroES;
        this->ESAtual = 0;
        this->tempoRestante = tempoBurst;
        this->prioridade = ALTA;
        this->frenteFilaES = false;
        this-> tempoprasair = 0;
    }
    Processo(){
        tempoprasair = 0;
    }

    void decrementaTempoRestante(int tempo){
      
      this->tempoRestante -= tempo;
    }

    void reset(){
        this->tempoRestante = this->tempoBurst;
    }
};

class Escalonador{
    queue<Processo> Q0;
    queue<Processo> Q1; 
    queue<Processo> Q2;
    queue<Processo> filaES;
    int tempoAtual;
    vector<Processo> todosProcessos;
    int quantum;

    public:
    
    Escalonador(){
        this->tempoAtual = 0;
        this->quantum = 0;
    }

    void adicionarProcesso(const Processo& p){
        Q0.push(p);
    }
    
    void voltaprocesso(Processo p){
        if(p.prioridade == ALTA){
            Q0.push(p);
        }else if(p.prioridade == MEDIA){
            Q1.push(p);
        }else if(p.prioridade == BAIXA){
            Q2.push(p);
        }
        else if(p.prioridade == ES){
            filaES.push(p);
            p.ESAtual++;
        }
    }

    

    bool processosprontos()//verifica se todos os processos estão prontos
    {
        vector<Processo> ::  iterator it;
        for(it = todosProcessos.begin(); it != todosProcessos.end(); it++){
            if(it->prioridade !=PRONTO){
                return false;
            }
        }
        return true;
    }

    bool cpuociosa(){
        vector<Processo> ::  iterator it;
        for(it = todosProcessos.begin(); it != todosProcessos.end(); it++){
            if(it->prioridade !=ES){
                return false;
            }
        }
        return true;
    }

    void sairES(){
        if(!filaES.empty()){
            Processo p = filaES.front();
            if(p.tempoprasair>=30){
                filaES.pop();
                p.prioridade = ALTA;
                voltaprocesso(p);
            }
    }
    }

    void tempodeespera(){
        if(!filaES.empty()){
            Processo p = filaES.front();
            p.tempoprasair++;
        }
    }

    void simular(){
        Processo p;
        while(!processosprontos()){
            sairES();
            if(!Q0.empty()){
                Processo p = Q0.front();
                quantum =10;
                int tempoQ0 = 0;
                p.tempoRestante--;
                tempoQ0++;
                if(p.tempoRestante == 0 && p.ESAtual <= p.numeroES){
                    cout << "Processo " << p.id << " foi enviado para fila e entrada e saida no tempo de "<< tempoAtual<< endl;
                    p.prioridade=ES;
                    voltaprocesso(p);
                    p.reset();
                }
                else if(tempoQ0 == quantum){
                    cout<<"Processo " << p.id << " executou por " << tempoQ0 << "ms e foi para a fila Q1" << endl;
                    p.prioridade = MEDIA;
                    voltaprocesso(p);
                }
                else if(p.tempoRestante == 0 && p.ESAtual > p.numeroES){
                    cout << "Processo " << p.id << " terminou no tempo de " << tempoAtual<< endl;
                    p.prioridade = PRONTO;
                    voltaprocesso(p);
                }

            }
            else if(!Q1.empty()){
                Processo p = Q1.front();
                quantum = 15;
            }
            else if(!Q2.empty()){
                Processo p = Q2.front();
            }
            tempoAtual++;
            tempodeespera();
        }
        }

};

int main() {
    Escalonador escalonador;
    int totalProcessos;
    cout << "Digite o total de processos: ";
    cin >> totalProcessos;
    cout << endl;
    for (int i = 0; i < totalProcessos; ++i) {
        int tempoBurst, numeroES;
        cout << "Digite a duracao do surto de CPU do processo " << (i) << ": ";
        cin >> tempoBurst;
        cout << "Digite o numero de operacoes de E/S do processo " << (i) << ": ";
        cin >> numeroES;
        cout << endl;
        escalonador.adicionarProcesso(Processo(i, tempoBurst, numeroES));
    }
    
    return 0;
}
