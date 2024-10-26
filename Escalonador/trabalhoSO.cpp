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
    list<Processo> todosProcessos;

public:
    Escalonador() : tempoAtual(0) {}

    void adicionarProcesso(const Processo& p) {
        todosProcessos.push_back(p);
        Q0.push(&todosProcessos.back()); // Adiciona o endereço do processo
}

    void voltaprocesso(Processo* p) {
        if (p->prioridade == ALTA) {
            Q0.push(p);
        } else if (p->prioridade == MEDIA) {
            Q1.push(p);
        } else if (p->prioridade == BAIXA) {
            Q2.push(p);
        } else if (p->prioridade == ES) {
            filaES.push(p);
            p->ESAtual++;
        }
    }

    bool processosprontos() {
        for (auto& processo : todosProcessos) {
            if (processo.prioridade != PRONTO) {
                return false;
            }
        }
        return true;
    }

    bool cpuociosa() {
        for (auto& processo : todosProcessos) {
            if (processo.prioridade != ES) {
                return false;
            }
        }
        return true;
    }

    void sairES() {
        if (!filaES.empty()) {
            Processo* p = filaES.front();
            if (p->tempoprasair >= 30) {
                filaES.pop();
                p->prioridade = ALTA;
                voltaprocesso(p);
                p->tempoprasair = 0;
                cout << "Processo " << p->id << " saiu da fila de entrada e saida no tempo de " << tempoAtual - 1 << endl;
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
            sairES();
            if (!Q0.empty()) {
                Processo* p = Q0.front();
                int quantum = 10;
                p->tempoRestante--;
                tempoQ0++;
                cout << "Processo " << p->id << " sendo executado na fila Q0 no tempo de " << tempoAtual << endl;
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
                cout << "Processo " << p->id << " sendo executado na fila Q1 no tempo de " << tempoAtual << endl;
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
                cout << "Processo " << p->id << " sendo executado na fila Q2 no tempo de " << tempoAtual << endl; 
                if (p->tempoRestante <= 0 && p->ESAtual < p->numeroES) {
                    cout << "Processo " << p->id << " foi enviado para fila de entrada e saida no tempo de " << tempoAtual << endl;
                    p->prioridade = ES;
                    voltaprocesso(p);
                    p->reset();
                    Q2.pop();
                } else if (p->tempoRestante <= 0 && p->ESAtual >= p->numeroES) {
                    cout << "Processo " << p->id << " pronto no tempo de " << tempoAtual << endl;
                    p->prioridade = PRONTO;
                    Q2.pop();
                }
                
            } else if (cpuociosa()) {
                cout << "CPU ociosa no tempo de " << tempoAtual << endl;
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
    cout<< "Grafico de Gantt: " << endl<<endl;
    escalonador.simular();
    return 0;
}
