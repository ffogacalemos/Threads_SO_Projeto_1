// Aluno: Fabricio Fogaça da Silva Lemos
// Disciplina: Sistemas Operacionais
// Professor: Dr. Marcos Paulo Moro
// FACET - UFGD
// 
// Intuito do código: Contar através de Threads os números primos gerados aleatoriamente em uma matriz
// Foi considerado o modo serial sendo nThreads = 1 e a submatriz = matriz
//

#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <vector>
#include <time.h>
#include <vector>
#include <math.h>
#include <limits>
#undef max
//undef necessário para pode fazer o tratamento do cin com a função cin.ignore()

using namespace std;

typedef struct {
    int end_inicial[2];
    int end_final[2];
    bool calculada;
}SUBMATRIZ;

typedef struct {
    int id;
    int end_inicial[2];
    int end_final[2];
    int n_sub;
    int tam_matriz_c;
    int tam_matriz_l;
}PARAM;

int** MATRIZ;
SUBMATRIZ* SUB_MATRIZ;

HANDLE hMutex_01;
HANDLE hMutex_02;


int PRIMOS = 0;

void menu();
int chk_int();
void criar_matriz(int, int);
int preencher_matriz(int, int, int);
int sub(int, int, int, int);
bool VER_PRIMO(int);
void CONTAR_PRIMO(void*);
void CRIAR_THREAD(int, int, int, int, vector<HANDLE>&, vector<PARAM>&);


int main()
{
    hMutex_01 = CreateMutex(NULL, FALSE, NULL);
    hMutex_02 = CreateMutex(NULL, FALSE, NULL);

    int parada = 1, gerado = 0, repetiu_threads = 0;
    int tam_matriz_l = 0, tam_matriz_c = 0, semente = 0, nThreads = 0, tam_sub_l= 0, tam_sub_c = 0, quant_sub = 0;
    time_t tempo_inicial = NULL, tempo_final = NULL;

    vector<HANDLE> hThread;
    vector<PARAM> vetorparametros;
    PARAM parametros;

    while (parada == 1) {
        int opc;
        menu();
        while (!(cin >> opc)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Tente um valor valido" << endl;
            menu();
        }

        switch (opc) {
            case 1:
                if (tam_matriz_l != 0 && tam_matriz_c != 0) {
                    for (int i = 0; i < tam_matriz_l; i++)
                        free(MATRIZ[i]);
                    free(MATRIZ);
                }

                cout << "Digite a quantidade de linhas" << endl;
                tam_matriz_l = chk_int();
                if (tam_matriz_l < 0) {
                    cout << "Digite um valor valido" << endl;
                    tam_matriz_l = chk_int();
                }

                cout << "Digite a quantidade de colunas" << endl;
                tam_matriz_c = chk_int();
                if (tam_matriz_c < 0) {
                    cout << "Digite um valor valido" << endl;
                    tam_matriz_c = chk_int();
                }

                criar_matriz(tam_matriz_l, tam_matriz_c);
                gerado = 0;
                break;

            case 2:
                cout << "Digite a semente" << endl;
                semente = chk_int();
                break;

            case 3:
                if ((tam_matriz_c > 0 && tam_matriz_l > 0) && semente != 0) {
                    gerado = preencher_matriz(tam_matriz_l, tam_matriz_c, semente);
                }
                else {
                    cout << "Primeiro digite a quantidade de linhas e colunas da matriz e/ou a semente" << endl;
                }
                break;

            case 4:
                if (tam_matriz_c == 0 && tam_matriz_l == 0 && gerado == 0) {
                    cout << "Primeiro crie uma Matriz e/ou preencha-a" << endl;
                    break;
                }
                do {
                    cout << "Digite a quantidade de linhas da submatriz" << endl;
                    tam_sub_l = chk_int();
                    while (tam_sub_l < 0) {
                        cout << "Digite um valor valido" << endl;
                        tam_sub_l = chk_int();
                    }

                    cout << "Digite a quantidade de colunas da submatriz" << endl;
                    tam_sub_c = chk_int();
                    while (tam_sub_c < 0) {
                        cout << "Digite um valor valido" << endl;
                        tam_sub_c = chk_int();
                    }
                    if((tam_sub_l * tam_sub_c) > (tam_matriz_l * tam_matriz_c))
                        cout << "Digite um valor valido" << endl;
                } while ((tam_sub_l*tam_sub_c)>(tam_matriz_l*tam_matriz_c));

                quant_sub = sub(tam_sub_l, tam_sub_c, tam_matriz_l, tam_matriz_c);
                break;
            case 5:
                if (nThreads != 0) {
                    for (int i = 0; i < nThreads; i++) {
                        CloseHandle(hThread[i]); 
                    }
                    for (int i = 0; i < nThreads; i++) {
                        hThread.pop_back();
                        vetorparametros.pop_back();
                    }

                }
                if (nThreads != 0 && quant_sub != 0)
                    quant_sub = sub(tam_sub_l, tam_sub_c, tam_matriz_l, tam_matriz_c);

                if (tam_sub_c == 0 && tam_sub_l == 0 && tam_matriz_l == 0 && tam_matriz_c == 0) {
                    cout << "Primeiro digite a Matriz e Submatriz" << endl;
                    break;
                }
                cout << "Digite a quantidade de threads" << endl;
                nThreads = chk_int();
                while (nThreads < 0) {
                    cout << "Digite um valor valido" << endl;
                    nThreads = chk_int();
                }

                CRIAR_THREAD(nThreads, quant_sub, tam_matriz_l, tam_matriz_c, hThread, vetorparametros);
                repetiu_threads = 0;
                break;
            case 6:
                if (gerado == 0) {
                    cout << "Primeiro gere a matriz" << endl;
                    break;
                }
                if (nThreads == 0) {
                    cout << "Primeiro Defina as Threads" << endl;
                    break;
                }
                if (repetiu_threads == 1) {
                    if (nThreads != 0) {
                        for (int i = 0; i < nThreads; i++) {
                            CloseHandle(hThread[i]);
                        }
                        for (int i = 0; i < nThreads; i++) {
                            hThread.pop_back();
                            vetorparametros.pop_back();
                        }

                    }
                    quant_sub = sub(tam_sub_l, tam_sub_c, tam_matriz_l, tam_matriz_c);
                    CRIAR_THREAD(nThreads, quant_sub, tam_matriz_l, tam_matriz_c, hThread, vetorparametros);
                }
                if ((tam_matriz_c * tam_matriz_l) < (tam_sub_c * tam_sub_l)) {
                    cout << "Digite uma submatriz valida primeiro: Submatriz > Matriz" << endl;
                    break;
                }

                PRIMOS = 0;
                tempo_inicial = time(0);

                for (int i = 0; i < nThreads; i++)
                    ResumeThread(hThread[i]);

                WaitForMultipleObjects(nThreads, hThread.data(), TRUE, INFINITE);
                
                tempo_final = time(0);
                repetiu_threads = 1;
                break;
            case 7:
                cout << "O tempo da ultima execucao foi: " << (tempo_final - tempo_inicial) << endl;
                cout << "O total de primos encontrados foi: " << PRIMOS << endl;
                break;
            case 8:
                for (int i = 0; i < nThreads; i++) {
                    CloseHandle(hThread[i]);
                }
                for (int i = 0; i < tam_matriz_l; i++)
                    free(MATRIZ[i]);
                free(MATRIZ);

                free(SUB_MATRIZ);

                parada = 0;
                break;
            default:
                cout << "Opcao inexistente" << endl;
                break;
        }
    }
}
//Menu do sistema
void menu() {
    cout << "--------------------------------------------------MENU--------------------------------------------------" << endl;
    cout << "Digite o numero da operacao desejada" << endl;
    cout << "1 - Defina o tamanho da Matriz\n2 - Defina a semente para o gerador de numeros aleatorios" << endl
         << "3 - Preencha a matriz com numeros aleatorios\n4 - Defina o tamanho das submatrizes\n5 - Defina o numero de Threads" << endl
         << "6 - Executar\n7 - Visualizar o tempo de execucao e quantidade de numeros primos\n8 - Encerrar\n";
}

//função para fazer tratamento do cin caso não seja digitado um inteiro pelo usuário
int chk_int() {
    int n;
    while (!(cin >> n)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Tente um valor valido\n:";
    }
    return n;
}
//função para criar a matriz dinâmica
void criar_matriz(int a, int b) {
    MATRIZ = new int* [a];
    for (int i = 0; i < a; i++) {
        MATRIZ[i] = new int[b];
    }
}
//preenche a matriz com números aleatórios utilizando a semente especificada
int preencher_matriz(int a, int b, int semente) {
    srand(semente);

    for (int i = 0; i < a; i++) {
        for (int j = 0; j < b; j++) {
            MATRIZ[i][j] = rand() % 100000000;
        }
    }
    return 1;
}
//cria os indices das submatrizes e retorna o total de submatrizes criadas
int sub(int linha_sub, int coluna_sub, int linha_matriz, int coluna_matriz){
    int contador_subs = 0, posi = 0, quantidade_inteira = 0;
    int total_sub = linha_sub * coluna_sub;
    int total_matriz = linha_matriz * coluna_matriz;
    float quantidade = float(total_matriz)/float(total_sub);


    if (floor(quantidade) != quantidade)
        quantidade_inteira = floor(quantidade);
    else
        quantidade_inteira = (floor(quantidade) + 1);

    SUB_MATRIZ = new SUBMATRIZ[quantidade_inteira];
    for (int i = 0; i < linha_matriz; i++) {
        for (int j = 0; j < coluna_matriz; j++) {
            if (contador_subs + 1 == quantidade_inteira) {

                SUB_MATRIZ[contador_subs].end_inicial[0] = i;
                SUB_MATRIZ[contador_subs].end_inicial[1] = j;

                SUB_MATRIZ[contador_subs].end_final[0] = linha_matriz - 1;
                SUB_MATRIZ[contador_subs].end_final[1] = coluna_matriz - 1;
                SUB_MATRIZ[contador_subs].calculada = false;

                contador_subs++;
                break;
            }
            if (posi == 0 && contador_subs < quantidade_inteira) {
                SUB_MATRIZ[contador_subs].end_inicial[0] = i;
                SUB_MATRIZ[contador_subs].end_inicial[1] = j;
                SUB_MATRIZ[contador_subs].calculada = false;
            }
            if (posi == total_sub - 1 && contador_subs < quantidade_inteira) {
                SUB_MATRIZ[contador_subs].end_final[0] = i;
                SUB_MATRIZ[contador_subs].end_final[1] = j;
                SUB_MATRIZ[contador_subs].calculada = false;
                posi = 0;
                contador_subs++;
            }
            else
                posi++;
        }
    }
   /* for (int i = 0; i < quantidade_inteira; i++) {
        cout << SUB_MATRIZ[i].end_inicial[0] << ":" << SUB_MATRIZ[i].end_inicial[1] << endl;
        cout << SUB_MATRIZ[i].end_final[0] << ":" << SUB_MATRIZ[i].end_final[1] << endl;
    }*/
    return quantidade_inteira;
}

//verifica se o número é primo ou não usando a relação de que os números primos maiores que 3 sempre são de ordem 6k+-1, código retirado de 
// https://en.wikipedia.org/wiki/Primality_test
bool VER_PRIMO(int primo) {
    if (primo == 2 || primo == 3)
        return true;

    if (primo <= 1 || primo % 2 == 0 || primo % 3 == 0)
        return false;

    for (int i = 5; i * i <= primo; i += 6){
        if (primo % i == 0 || primo % (i + 2) == 0)
            return false;
    }

    return true;
}

//funcao para contar os numeros primos utilizando threads
void CONTAR_PRIMO(void* parametros) {
    PARAM* param = (PARAM*)parametros;
    int total_primos = 0, total_n = 0;
    int j, i;
    bool parada_1 = false, parada_2 = false;

    while (parada_1 == false) {

       WaitForSingleObject(hMutex_01, INFINITE);

        parada_2 = false;
        for (int i = 0; i < param->n_sub; i++) {
            if (SUB_MATRIZ[i].calculada == false) {
                param->end_final[0] = SUB_MATRIZ[i].end_final[0];
                param->end_final[1] = SUB_MATRIZ[i].end_final[1];

                param->end_inicial[0] = SUB_MATRIZ[i].end_inicial[0];
                param->end_inicial[1] = SUB_MATRIZ[i].end_inicial[1];
                SUB_MATRIZ[i].calculada = true;

                i = param->n_sub;
            }
            else
                if ((i + 1) >= param->n_sub) {
                    parada_1 = true;
                }
        }
       /*cout << param->end_inicial[0] << ":" << param->end_inicial[1] << endl;
       cout << param->end_final[0] << ":" << param->end_final[1] << endl;*/

       ReleaseMutex(hMutex_01);

        if (parada_1 == false && parada_2 == false) {
            i = param->end_inicial[0];
            j = param->end_inicial[1];

            while (parada_2 == false){
                if (VER_PRIMO(MATRIZ[i][j]) == true)
                    total_primos++;


                if (i == param->end_final[0] && j == param->end_final[1])
                    parada_2 = true;
                else {
                    if (j < param->tam_matriz_c)
                        j++;
                    else {
                        j = 0;
                        i++;
                    }
                }
                total_n++;
            }
        }
    }

    WaitForSingleObject(hMutex_02, INFINITE);
    PRIMOS += total_primos;
    ReleaseMutex(hMutex_02);
    _endthread();
}

//Cria as Threads
void CRIAR_THREAD(int nThreads, int quant_sub, int tam_matriz_l, int tam_matriz_c, vector<HANDLE>& hThread, vector<PARAM>& vetorparametros) {
    PARAM parametros;

    for (int i = 0; i < nThreads; i++) {
        parametros.id = i;
        parametros.end_inicial[0] = NULL;
        parametros.end_inicial[1] = NULL;
        parametros.end_final[0] = NULL;
        parametros.end_final[1] = NULL;
        parametros.n_sub = quant_sub;
        parametros.tam_matriz_l = tam_matriz_l;
        parametros.tam_matriz_c = tam_matriz_c;

        vetorparametros.push_back(parametros);
    }
    for (int i = 0; i < nThreads; i++) {
        hThread.push_back(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CONTAR_PRIMO, &vetorparametros[i], CREATE_SUSPENDED, NULL));
    }
}