#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

struct Processo {
    int id;
    int tempodeburst;
    int prioridade; // 0 para Q0, 1 para Q1, 2 para Q2
    int numeroES; // Número de operações de E/S
    int esExecutadas; // Número de operações de E/S já executadas
    int tempoRestante;
    int tempodeentrada; // Tempo de entrada do processo na fila de E/S

    Processo(int id, int tempodeburst, int numeroES) 
        : id(id), tempodeburst(tempodeburst), prioridade(0), numeroES(numeroES), esExecutadas(0), tempoRestante(tempodeburst), tempodeentrada(0) {}
    Processo() {}

};

struct Execucao {
    int id; // -1 indica CPU ociosa
    int inicio;
    int fim;
    string filaName;
    Execucao(int id, int inicio, int fim, string filaName) : id(id), inicio(inicio), fim(fim), filaName(filaName) {}
};

class Escalonador {
private:
    queue<Processo> Q0;
    queue<Processo> Q1;
    queue<Processo> Q2;
    queue<Processo> filaES; // Fila de processos aguardando E/S
    queue<Processo> filaProntos; // Fila de processos prontos
    int tempoAtual;
    vector<Execucao> execucoes; // Vetor para armazenar execuções
    bool disponivel = true;

    void roundrobin(queue<Processo>& fila, int quantum, int proximaPrioridade){
        if(fila.empty()) return;
        Processo processo = fila.front();
        string filaName;
        if(processo.prioridade == 0 )
            filaName = "Q0";
        else if( processo.prioridade==1)
            filaName = "Q1";
        fila.pop();
        int tempoExec = min(quantum, processo.tempoRestante);
        int inicio = tempoAtual;
        processo.tempoRestante -= tempoExec;
        tempoAtual += tempoExec;
        int fim = tempoAtual;
        execucoes.emplace_back(processo.id, inicio, fim, filaName);
        cout << "Executando o Processo " << processo.id << " por " << tempoExec << "ms (Restante: " << processo.tempoRestante << "ms)\n";
        if(processo.tempoRestante > 0){
            if(processo.esExecutadas < processo.numeroES){
                processo.esExecutadas++;
                processo.tempodeentrada = tempoAtual;
                if(processo.prioridade<2)
                    processo.prioridade+=1;
                filaES.push(processo);
            } else {
                processo.prioridade = proximaPrioridade;
                if(proximaPrioridade == 1){
                    Q1.push(processo);
                } else if(proximaPrioridade == 2){
                    Q2.push(processo);
                }
            }
        } else {
            cout << "Processo " << processo.id << " concluido no tempo " << tempoAtual << "ms\n";
            filaProntos.push(processo);
        }
    }

    bool cpuociosa(){
        if(Q0.empty() && Q1.empty() && Q2.empty() && !filaES.empty()) {
            cout << "CPU ociosa no tempo de " << tempoAtual << " milisegundos" << endl;
            return true;
        }
        return false;
    }

    void executarFCFS(queue<Processo>& fila){
        if(fila.empty()) return;
        Processo processo = fila.front();
        fila.pop();
        int inicio = tempoAtual;
        tempoAtual += processo.tempoRestante;
        int fim = tempoAtual;
        string filaName = "Q2";
        execucoes.emplace_back(processo.id, inicio, fim, filaName);
        cout << "Executando o Processo " << processo.id << " por " << processo.tempoRestante << "ms (Concluido)\n";
        processo.tempoRestante = 0;
        cout << "Processo " << processo.id << " concluido no tempo " << tempoAtual << "ms\n";
    }

    void executarES(){
        if(!filaES.empty()){
            Processo processo = filaES.front();
            if(tempoAtual - processo.tempodeentrada >= 30){
                filaES.pop();
                cout << "Processo " << processo.id << " finalizou E/S e voltou para a fila de prontos no tempo " << tempoAtual << "ms\n";
                if(processo.prioridade == 0) {
                    Q0.push(processo);
                } else if (processo.prioridade == 1) {
                    Q1.push(processo);
                } else {
                    Q2.push(processo);
                }
            }
        }
    }

public:
    Escalonador() : tempoAtual(0){}

    void adicionarProcesso(const Processo& processo) {
        Q0.push(processo);
    }

    void simular() {
        while (!Q0.empty() || !Q1.empty() || !Q2.empty() || !filaES.empty()) {
            if (!Q0.empty()) {
                roundrobin(Q0, 10, 1);
            } else if (!Q1.empty()) {
                roundrobin(Q1, 15, 2);
            } else if (!Q2.empty()) {
                executarFCFS(Q2);
            }

            if (!filaES.empty()) {
                executarES();
            }

            if(cpuociosa()) {
                Processo processo = filaES.front();
                int tempoIncremento = 30 - (tempoAtual - processo.tempodeentrada);
                int inicio = tempoAtual;
                tempoAtual += tempoIncremento;
                int fim = tempoAtual;
                execucoes.emplace_back(-1, inicio, fim,""); // -1 indica CPU ociosa
                filaES.pop();
                cout << "Processo " << processo.id << " finalizou E/S e voltou para a fila de prontos no tempo " << tempoAtual << "ms\n";
                if(processo.prioridade == 0) {
                    Q0.push(processo);
                } else if (processo.prioridade == 1) {
                    Q1.push(processo);
                } else {
                    Q2.push(processo);
                }
            }
        }
    }

    void graficodeGantt() {
        cout << endl<< "Grafico de Gantt:\n";
        
        // Imprimindo a linha do tempo
        cout << "Tempo/ Processo/ Fila:   ";
        for (const auto& exec : execucoes) {
            for (int i = exec.inicio; i < exec.fim; ++i) {
                cout << "| " << i << "/";
                 if (exec.id == -1) {
                    cout << " CPUO";
                } else {
                    cout << " P" << exec.id<< "/ "<< exec.filaName;
                }
                cout << " ";
                
            }
        }
        cout << "|"<<endl;
        cout<< "Todos os processos prontos no tempo: " << execucoes.back().fim -1  << "\n";
        
        
    }
};

int main() {
    Escalonador escalonador;

    int totalProcessos;
    cout << "Digite o total de processos: ";
    cin >> totalProcessos;
    cout<<endl;

    for (int i = 0; i < totalProcessos; ++i) {
        int tempoBurst, numeroES;
        cout << "Digite a duracao do surto de CPU do processo " << (i + 1) << ": ";
        cin >> tempoBurst;
        cout << "Digite o numero de operacoes de E/S do processo " << (i + 1) << ": ";
        cin >> numeroES;
        cout << endl;

        escalonador.adicionarProcesso(Processo(i + 1, tempoBurst, numeroES));
    }

    escalonador.simular();
    escalonador.graficodeGantt();
    return 0;
}
