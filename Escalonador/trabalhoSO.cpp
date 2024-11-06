/*  Al Belchior 22023
    Al Schuinki 22040
    Al Giovanna Fioravanso 220
    Al Caruba 22035
    Al Andrade 22032*/

#include <iostream>
#include <queue>
#include <list>
#include <algorithm>
#include <string>

using namespace std;

enum Prioridade {
    ALTA,
    MEDIA,
    BAIXA,
    ES,
    PRONTO
};

struct Gantt{
    int id;
    string fila;
    int tempoAtual;
    
    Gantt() {}

    Gantt(int id, string fila, int tempoAtual) : id(id), fila(fila), tempoAtual(tempoAtual) {}
};

struct Processo {
    int id;
    int tempoBurst;
    int numeroES;
    int ESAtual;
    int tempoRestante;
    Prioridade prioridade;
    int tempoprasair;

    Processo(int id, int tempoBurst, int numeroES) {
        this->id = id;
        this->tempoBurst = tempoBurst;
        this->numeroES = numeroES;
        this->ESAtual = 0;
        this->tempoRestante = tempoBurst;
        this->prioridade = ALTA;
        this->tempoprasair = 0;
    }
    Processo() {
    }
    
    void reset() {
        this->tempoRestante = this->tempoBurst;
    }
};

class Escalonador {
    queue<Processo*> Q0;
    queue<Processo*> Q1;
    queue<Processo*> Q2;
    queue<Processo*> filaES;
    queue<Processo*> filaPronto;
    int tempoAtual;
    bool primeiro= true;
    list<Processo> todosProcessos;
    vector<Gantt> gantt;

public:
    Escalonador() : tempoAtual(0) {}

    void adicionarProcesso(const Processo& p) {

        todosProcessos.push_back(p);
        Q0.push(&todosProcessos.back()); 
}
void adicionarGantt(const Gantt& g) {

        gantt.push_back(g);
    }
void conttGantt(){

    int inicioTempo = -1; 
    int fimTempo = -1;    
    cout<<"---------------------------------"<<endl;
    for (size_t i = 0; i < gantt.size(); i++) {
        if (i == 0 || (gantt[i].id == gantt[i - 1].id && gantt[i].fila == gantt[i - 1].fila)) {
            
            if (inicioTempo == -1) {
                inicioTempo = gantt[i].tempoAtual;
            }
            fimTempo = gantt[i].tempoAtual;
        } else {
            
            if (inicioTempo != -1 && gantt[i-1].id != 99) {
                cout << "|Processo " << gantt[i - 1].id <<  " -> Inicio: " << inicioTempo << ", Fim: " << fimTempo <<"|"<< endl<< "-----------------------------------" << endl;
            }
            else if(inicioTempo != -1 && gantt[i-1].id == 99 ){
                cout << "|CPU ociosa -> Inicio: " << inicioTempo << ", Fim: " << fimTempo <<"|"<< endl<< "-----------------------------------" << endl;
            }
            //reset
            inicioTempo = gantt[i].tempoAtual;
            fimTempo = gantt[i].tempoAtual;
        }
    }
    // ultima ocorrencia
    if (inicioTempo != -1) {
        cout << "|Processo " << gantt.back().id << "->  Inicio: " << inicioTempo << ", Fim: " << fimTempo <<"|"<< endl<< "-----------------------------------" << endl;
    }
}

void voltaprocesso(Processo* p) {

        if (p->prioridade == ALTA) {
            Q0.push(p);
        } else if (p->prioridade == MEDIA) {
            Q1.push(p);
        } else if (p->prioridade == BAIXA) {
            Q2.push(p);
        } else if (p->prioridade == ES) {
            if(filaES.empty()) primeiro = true;
            filaES.push(p);
            p->ESAtual++;
        }
    }

    bool processosprontos() {

        for (auto& processo : todosProcessos) {
            if (processo.prioridade != PRONTO)  return false;
        }
        return true;
    }

    bool cpuociosa() {

        for (auto& processo : todosProcessos) {
            if (processo.prioridade != ES && processo.prioridade!=PRONTO) return false;
        }
        return true;
    }

    void sairES() {

        if (!filaES.empty()) {
            Processo* p = filaES.front();

            if (p->tempoprasair >= 30 && !primeiro) {
                filaES.pop();
                p->prioridade = ALTA;
                voltaprocesso(p);
                p->tempoprasair = 0;
                cout << "Processo " << p->id << " saiu da fila de entrada e saida no tempo de " << tempoAtual - 1 << endl;
            }
            else if (p->tempoprasair > 30) {
                filaES.pop();
                p->prioridade = ALTA;
                voltaprocesso(p);
                p->tempoprasair = 0;
                cout << "Processo " << p->id << " saiu da fila de entrada e saida no tempo de " << tempoAtual - 1 << endl;
                primeiro = false;
            }
        }
    }

    void tempodeespera() {

        if (!filaES.empty()) {
            Processo* p = filaES.front();
            p->tempoprasair++;
        }
    }

    void simular() {
        int tempoQ0 = 0;
        int tempoQ1 = 0;

        while (!processosprontos()) {
            tempodeespera();
            sairES();

            if (!Q0.empty()) {
                Processo* p = Q0.front();
                int quantum = 10;
                p->tempoRestante--;
                tempoQ0++;
                adicionarGantt(Gantt{p->id, "Q0", tempoAtual});
                if (p->tempoRestante == 0 && p->ESAtual < p->numeroES) {
                    cout << "Processo " << p->id << " foi enviado para fila de entrada e saida no tempo de " << tempoAtual << endl;
                    p->prioridade = ES;
                    voltaprocesso(p);
                    p->reset();
                    tempoQ0 = 0;
                    Q0.pop();
                } else if (tempoQ0 == quantum) {
                    cout << "Processo " << p->id << " executou por " << tempoQ0 << "ms e foi para a fila Q1" << endl;
                    p->prioridade = MEDIA;
                    voltaprocesso(p);
                    tempoQ0 = 0;
                    Q0.pop();
                } else if (p->tempoRestante == 0 && p->ESAtual >= p->numeroES) {
                    cout << "Processo " << p->id << " pronto no tempo de " << tempoAtual << endl;
                    p->prioridade = PRONTO;
                    tempoQ0 = 0;
                    Q0.pop();
                }

            } else if (!Q1.empty()) {
                Processo* p = Q1.front();
                int quantum = 15;
                p->tempoRestante--;
                tempoQ1++;
                adicionarGantt(Gantt{p->id, "Q1", tempoAtual});
                if (p->tempoRestante == 0 && p->ESAtual < p->numeroES) {
                    cout << "Processo " << p->id << " foi enviado para fila de entrada e saida no tempo de " << tempoAtual << endl;
                    p->prioridade = ES;
                    voltaprocesso(p);
                    p->reset();
                    tempoQ1 = 0;
                    Q1.pop();
                } else if (tempoQ1 == quantum) {
                    cout << "Processo " << p->id << " executou por " << tempoQ1 << "ms e foi para a fila Q2" << endl;
                    p->prioridade = BAIXA;
                    voltaprocesso(p);
                    tempoQ1 = 0;
                    Q1.pop();
                } else if (p->tempoRestante == 0 && p->ESAtual >= p->numeroES) {
                    cout << "Processo " << p->id << " pronto no tempo de " << tempoAtual << endl;
                    p->prioridade = PRONTO;
                    tempoQ1 = 0;
                    Q1.pop();
                }

            } 
            else if (!Q2.empty()) {
                Processo* p = Q2.front();
                p->tempoRestante--;
                adicionarGantt(Gantt{p->id, "Q2",   tempoAtual});
                if (p->tempoRestante <= 0 && p->ESAtual < p->numeroES) {
                    cout << "Processo " << p->id << " foi enviado para fila de entrada e saida no tempo de " << tempoAtual << endl;
                    p->prioridade = ES;
                    voltaprocesso(p);
                    p->reset();
                    Q2.pop();
                } else if (p->tempoRestante <= 0 && p->ESAtual >= p->numeroES) {
                    cout << "Processo " << p->id << " pronto no tempo de " << tempoAtual  << endl;
                    p->prioridade = PRONTO;
                    Q2.pop();
                }
                
            } else if (cpuociosa()) {
                adicionarGantt(Gantt{99, "CPUO", tempoAtual});
            }
            tempoAtual++;
            
        }
        cout<< "Tempo total de processamento: "<< tempoAtual<< "ms."<< endl;
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

    escalonador.simular();
    cout<<endl<< "GRAFICO DE GANTT: " << endl<<endl;
    escalonador.conttGantt();
    return 0;
}
