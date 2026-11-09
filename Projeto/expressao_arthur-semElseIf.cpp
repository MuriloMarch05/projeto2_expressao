
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits>
using namespace std;

struct Elemento{
    char Operador[10];
    float Numero;
    Elemento *Prox;
};

struct Fila{
    Elemento *Inicio;
    Elemento *Fim;
};

struct Pilha{
    Elemento *Topo;
};

void Ini_Fila(Fila *F){
    F->Inicio = NULL;
    F->Fim = NULL;
}

void Ini_Pilha(Pilha *P){
    P->Topo = NULL;
}

bool Fila_Vazia(Fila *F){
    return (F->Inicio == NULL);
}

bool Pilha_Vazia(Pilha *P){
    return (P->Topo == NULL);
}

Elemento * Criar_Elemento(const char* Op, float Num){
    Elemento *Ret;
    Ret = (Elemento *) malloc(sizeof(Elemento));
    if(Ret != NULL){
        strcpy(Ret->Operador, Op);
        Ret->Numero = Num;
        Ret->Prox = NULL;
    }
    return Ret;
}

// FIFO - ENQUEUE
void Enfileirar(Fila *F, const char* Op, float Num){
    Elemento *ptE;
    ptE = Criar_Elemento(Op, Num);
    if(ptE != NULL){
        if(Fila_Vazia(F)){
            F->Inicio = ptE;
        }
        else{
            F->Fim->Prox = ptE;
        }
        F->Fim = ptE;
    }
}

// ALTERAÇÃO: adiciona uma função para liberar todos os elementos da fila,
// evitando vazamentos de memória ao substituir ou encerrar uma expressão.
void Limpar_Fila(Fila *F){
    Elemento *Aux;
    while(!Fila_Vazia(F)){
        Aux = F->Inicio;
        F->Inicio = F->Inicio->Prox;
        free(Aux);
    }
    F->Fim = NULL;
}

// ALTERAÇÃO: corrige a atualização dos ponteiros Inicio e Fim durante a
// remoção de elementos, além de desvincular o nó removido da fila.
// FIFO - DEQUEUE
Elemento * Desenfileirar(Fila *F){
    Elemento *Ret = NULL;
    if(!Fila_Vazia(F)){
        Ret = F->Inicio;
        F->Inicio = F->Inicio->Prox;
        if(F->Inicio == NULL){
            F->Fim = NULL;
        }
        Ret->Prox = NULL;
    }
    return Ret;
}

// LIFO - PUSH
void Empilhar(Pilha *P, const char* Op, float Num){
    Elemento *ptE;
    ptE = Criar_Elemento(Op, Num);
    if(ptE != NULL){
        ptE->Prox = P->Topo;
        P->Topo = ptE;
    }
}

// ALTERAÇÃO: desvincula o elemento removido da pilha, evitando que ele
// continue apontando para os demais elementos após o desempilhamento.
// LIFO - POP
Elemento * Desempilhar(Pilha *P){
    Elemento *Ret = NULL;
    if(!Pilha_Vazia(P)){
        Ret = P->Topo;
        P->Topo = P->Topo->Prox;
        Ret->Prox = NULL;
    }
    return Ret;
}

// ALTERAÇÃO: adiciona uma função para liberar todos os elementos de uma
// pilha, garantindo a liberação da memória alocada dinamicamente.
void Limpar_Pilha(Pilha *P){
    Elemento *Aux;
    while(!Pilha_Vazia(P)){
        Aux = Desempilhar(P);
        free(Aux);
    }
}

// ALTERAÇÃO: adiciona a validação do balanceamento dos parênteses,
// identificando expressões com aberturas ou fechamentos incorretos.
bool Analisar_parenteses(const char exp[]){
    Pilha P;
    Ini_Pilha(&P);
    Elemento *Aux;

    for(int i=0; exp[i]!='\0'; i++){
        if(exp[i] == '('){
            Empilhar(&P, "(", 0);
        }
        else{
            if(exp[i] == ')'){
                if(Pilha_Vazia(&P)){
                    return false;
                }
                Aux = Desempilhar(&P);
                free(Aux);
            }
        }
    }

    bool valido = Pilha_Vazia(&P);

    while(!Pilha_Vazia(&P)){
        Aux = Desempilhar(&P);
        free(Aux);
    }

    return valido;
}

// ALTERAÇÃO: implementa a validação de números decimais, verificando
// pontos repetidos, ausência de algarismos e limites do vetor auxiliar.
// A função também passa a retornar um booleano para indicar erros.
bool Inserir_num(Fila *F, char exp[], int *i){
    char auxnum[100];
    int j = 0;
    bool ponto = false;
    bool temNumero = false;

    while(isdigit(exp[*i]) || exp[*i] == '.'){
        if(exp[*i] == '.'){
            if(ponto || !temNumero){
                return false;
            }
            ponto = true;
        }
        else{
            temNumero = true;
        }

        if(j >= 99){
            return false;
        }

        auxnum[j] = exp[*i];
        j++;
        (*i)++;
    }

    auxnum[j] = '\0';

    if(!temNumero || auxnum[j-1] == '.'){
        return false;
    }

    Enfileirar(F, "#", atof(auxnum));
    return true;
}

// ALTERAÇÃO: simplifica a identificação dos operadores, reconhecendo
// operadores relacionais de dois caracteres, como <=, >=, != e ==.
void Inserir_op(Fila *F, char exp[], int *i){
    char auxOp[3];
    auxOp[0] = exp[*i];
    auxOp[1] = '\0';

    if((exp[*i] == '<' || exp[*i] == '>' || exp[*i] == '!' || exp[*i] == '=') && exp[*i+1] == '='){
        auxOp[1] = '=';
        auxOp[2] = '\0';
        (*i)++;
    }

    Enfileirar(F, auxOp, 0);
}

// ALTERAÇÃO: passa a retornar a estrutura Fila completa e implementa
// validações de sintaxe, incluindo operadores em posições inválidas,
// números consecutivos, parênteses vazios e caracteres não permitidos.
// Em caso de erro, os elementos já inseridos são liberados.
Fila Digitar_exp(){
    char exp[100];
    cout << "\nDigite uma expressão numérica: ";
    cin.getline(exp, 100);

    Fila F;
    Ini_Fila(&F);

    if(!Analisar_parenteses(exp)){
        cout << "\nErro: parênteses desbalanceados!\n";
        return F;
    }

    bool EsperandoNumero = true;

    for(int i=0; exp[i]!='\0'; i++){
        if(isspace(exp[i])){
            continue;
        }

        if(isdigit(exp[i]) || exp[i] == '.'){
            if(!EsperandoNumero){
                cout << "\nErro: operador ausente entre números!\n";
                Limpar_Fila(&F);
                return F;
            }

            if(!Inserir_num(&F, exp, &i)){
                cout << "\nErro: número decimal inválido!\n";
                Limpar_Fila(&F);
                return F;
            }

            EsperandoNumero = false;
            i--;
        }
        else{
            if(exp[i] == '('){
                if(!EsperandoNumero){
                    cout << "\nErro: operador ausente antes do parêntese!\n";
                    Limpar_Fila(&F);
                    return F;
                }

                Enfileirar(&F, "(", 0);
                EsperandoNumero = true;
            }
            else{
                if(exp[i] == ')'){
                    if(EsperandoNumero){
                        cout << "\nErro: parêntese vazio ou operador incompleto!\n";
                        Limpar_Fila(&F);
                        return F;
                    }

                    Enfileirar(&F, ")", 0);
                    EsperandoNumero = false;
                }
                else{
                    if(strchr("+-*/<>!=", exp[i]) != NULL){
                        if(EsperandoNumero){
                            cout << "\nErro: operador em posição inválida!\n";
                            Limpar_Fila(&F);
                            return F;
                        }

                        Inserir_op(&F, exp, &i);
                        EsperandoNumero = true;
                    }
                    else{
                        cout << "\nErro: caractere inválido na expressão!\n";
                        Limpar_Fila(&F);
                        return F;
                    }
                }
            }
        }
    }

    if(EsperandoNumero && !Fila_Vazia(&F)){
        cout << "\nErro: expressão incompleta!\n";
        Limpar_Fila(&F);
    }

    return F;
}

// ALTERAÇÃO: passa a retornar um booleano para indicar o sucesso da
// operação e utiliza um ponteiro para armazenar o resultado. Também
// corrige a comparação de igualdade e trata a divisão por zero.
bool Calculo_parcial(float num1, float num2, char op[], float *retorno){
    if(strcmp(op, "+") == 0){
        *retorno = num1 + num2;
    }
    else{
        if(strcmp(op, "-") == 0){
            *retorno = num1 - num2;
        }
        else{
            if(strcmp(op, "*") == 0){
                *retorno = num1 * num2;
            }
            else{
                if(strcmp(op, "/") == 0){
                    if(num2 == 0){
                        return false;
                    }
                    *retorno = num1 / num2;
                }
                else{
                    if(strcmp(op, "<") == 0){
                        *retorno = (num1 < num2);
                    }
                    else{
                        if(strcmp(op, ">") == 0){
                            *retorno = (num1 > num2);
                        }
                        else{
                            if(strcmp(op, "<=") == 0){
                                *retorno = (num1 <= num2);
                            }
                            else{
                                if(strcmp(op, ">=") == 0){
                                    *retorno = (num1 >= num2);
                                }
                                else{
                                    if(strcmp(op, "!=") == 0){
                                        *retorno = (num1 != num2);
                                    }
                                    else{
                                        if(strcmp(op, "=") == 0 || strcmp(op, "==") == 0){
                                            *retorno = (num1 == num2);
                                        }
                                        else{
                                            return false;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}

// ALTERAÇÃO: verifica os elementos diretamente pelos ponteiros.
bool Resolver_pilha(Pilha *PNum, Pilha *POp){
    if(PNum->Topo == NULL || PNum->Topo->Prox == NULL || Pilha_Vazia(POp)){
        return false;
    }

    Elemento *E1, *E2, *EOp;
    float num1, num2, resultado;
    char op[10];

    E1 = Desempilhar(PNum);
    E2 = Desempilhar(PNum);
    EOp = Desempilhar(POp);

    num2 = E1->Numero;
    num1 = E2->Numero;
    strcpy(op, EOp->Operador);

    free(E1);
    free(E2);
    free(EOp);

    if(!Calculo_parcial(num1, num2, op, &resultado)){
        return false;
    }

    Empilhar(PNum, "#", resultado);
    return true;
}

// ALTERAÇÃO: reorganiza a identificação da precedência dos operadores
// e passa a retornar -1 para operadores desconhecidos, permitindo
// identificar entradas inválidas durante o cálculo.
int preced(const char *Op){
    if(strcmp(Op, "<=") == 0 || strcmp(Op, ">=") == 0 || strcmp(Op, "<") == 0 || strcmp(Op, ">") == 0 || strcmp(Op, "!=") == 0 || strcmp(Op, "=") == 0 || strcmp(Op, "==") == 0){
        return 1;
    }

    if(strcmp(Op, "+") == 0 || strcmp(Op, "-") == 0){
        return 2;
    }

    if(strcmp(Op, "*") == 0 || strcmp(Op, "/") == 0){
        return 3;
    }

    return -1;
}

// ALTERAÇÃO: implementa o tratamento de parênteses e o controle da
// precedência dos operadores durante o cálculo. Passa a retornar um
// booleano para indicar sucesso ou falha, armazenando o resultado por
// referência. Também verifica a validade do cálculo e libera a memória
// utilizada pelas pilhas ao final da execução.
bool Calcular_exp(Fila *F, float *Resultado){
    if(Fila_Vazia(F)){
        return false;
    }

    Pilha PNum, POp;
    Ini_Pilha(&PNum);
    Ini_Pilha(&POp);

    Elemento *pto;
    bool valido = true;

    for(pto = F->Inicio; pto != NULL && valido; pto = pto->Prox){
        if(strcmp(pto->Operador, "#") == 0){
            Empilhar(&PNum, "#", pto->Numero);
        }
        else{
            if(strcmp(pto->Operador, "(") == 0){
                Empilhar(&POp, "(", 0);
            }
            else{
                if(strcmp(pto->Operador, ")") == 0){
                    while(!Pilha_Vazia(&POp) && strcmp(POp.Topo->Operador, "(") != 0){
                        if(!Resolver_pilha(&PNum, &POp)){
                            valido = false;
                            break;
                        }
                    }

                    if(valido){
                        if(Pilha_Vazia(&POp)){
                            valido = false;
                        }
                        else{
                            Elemento *Aux = Desempilhar(&POp);
                            free(Aux);
                        }
                    }
                }
                else{
                    int prioridadeAtual = preced(pto->Operador);

                    if(prioridadeAtual == -1){
                        valido = false;
                    }
                    else{
                        while(!Pilha_Vazia(&POp) && strcmp(POp.Topo->Operador, "(") != 0 && preced(POp.Topo->Operador) >= prioridadeAtual){
                            if(!Resolver_pilha(&PNum, &POp)){
                                valido = false;
                                break;
                            }
                        }

                        if(valido){
                            Empilhar(&POp, pto->Operador, 0);
                        }
                    }
                }
            }
        }
    }

    while(valido && !Pilha_Vazia(&POp)){
        if(strcmp(POp.Topo->Operador, "(") == 0){
            valido = false;
            break;
        }

        if(!Resolver_pilha(&PNum, &POp)){
            valido = false;
        }
    }

    if(valido && PNum.Topo != NULL && PNum.Topo->Prox == NULL){
        *Resultado = PNum.Topo->Numero;
    }
    else{
        valido = false;
    }

    Limpar_Pilha(&PNum);
    Limpar_Pilha(&POp);

    return valido;
}

// ALTERAÇÃO: inicializa corretamente a fila e implementa o gerenciamento
// da memória das expressões. Também atualiza a chamada de Calcular_exp(),
// apresenta mensagens para expressões inválidas e impede cálculos quando
// nenhuma expressão foi inserida.
void Menu(){
    int Op;
    Fila F;
    Ini_Fila(&F);
    float Resultado;

    do{
        cout << "\n[1] - Inserir expressão";
        cout << "\n[2] - Calcular expressão";
        cout << "\n[3] - Sair";
        cout << "\nOpção: ";

        cin >> Op;

        switch(Op){
            case 1:
                Limpar_Fila(&F);
                cin.ignore();
                F = Digitar_exp();
                break;

            case 2:
                if(Fila_Vazia(&F)){
                    cout << "\nNenhuma expressão foi inserida!\n";
                }
                else{
                    if(Calcular_exp(&F, &Resultado)){
                        cout << "\nO resultado da expressão é: " << Resultado << endl;
                    }
                    else{
                        cout << "\nErro: não foi possível calcular a expressão!\n";
                        cout << "Verifique a expressão e tente novamente.\n";
                    }
                }
                break;

            case 3:
                Limpar_Fila(&F);
                cout << "\nEncerrando o programa...\n";
                break;

            default:
                cout << "\nOpção inválida!\n";
                break;
        }
    }while(Op != 3);
}

int main(){
    Menu();
    return 0;
}
