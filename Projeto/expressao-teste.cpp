#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <cctype>

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
    if (Ret != NULL){
        strcpy(Ret->Operador, Op);
        Ret->Numero = Num;
        Ret->Prox = NULL;
    }
    return Ret;
}

void Enfileirar(Fila *F, const char* Op, float Num){
    Elemento *ptE;
    ptE = Criar_Elemento(Op, Num);
    if (ptE != NULL){
        if (Fila_Vazia(F)){
            F->Inicio = ptE;
        }
        else{
            F->Fim->Prox = ptE;
        }
        F->Fim = ptE;
    }   
}

void Inserir_num(Fila *F, char exp[], int *i){
    char auxnum[100];
    int j = 0;
    while(isdigit(exp[*i]) || (exp[*i] == '.' && isdigit(exp[*i+1]))){
        auxnum[j] = exp[*i];
        (*i)++;
        j++;
    }
    auxnum[j] = '\0';
    Enfileirar(F, "#", atof(auxnum));
    (*i)--; // Ajusta o incremento do loop principal
}

void Inserir_op(Fila *F, char exp[], int *i){
    char auxOp[3];
    int j = 0;
    // Captura operadores simples ou parênteses
    auxOp[0] = exp[*i];
    auxOp[1] = '\0';
    Enfileirar(F, auxOp, 0);
}

void Digitar_exp(Fila *F){
    char exp[100];
    cin.ignore();
    cout << "\nDigite uma expressao numerica: ";
    cin.getline(exp, 100);
    cout << endl;
    
    Ini_Fila(F);
    
    for(int i = 0; exp[i] != '\0'; i++){
        if(isspace(exp[i])) continue; // Ignora espaços em branco
        
        if(isdigit(exp[i])){
            Inserir_num(F, exp, &i);
        }
        else{
            Inserir_op(F, exp, &i);
        }
    }
}

Elemento * Desenfileirar(Fila *F){
    Elemento *Ret = NULL;
    if (!Fila_Vazia(F)){
        Ret = F->Inicio;
        if (F->Inicio == F->Fim){
            F->Inicio = NULL;
            F->Fim = NULL;
        }
        else{
            F->Inicio = F->Inicio->Prox;
        }
    }
    return Ret;
}

void Empilhar(Pilha *P, const char* Op, float Num){
    Elemento *ptE;
    ptE = Criar_Elemento(Op, Num);
    if(ptE != NULL){
        if (!Pilha_Vazia(P)){
            ptE->Prox = P->Topo;
        }
        P->Topo = ptE;
    }   
}

Elemento * Desempilhar(Pilha *P){
    Elemento *Ret = NULL;
    if (!Pilha_Vazia(P)){
        Ret = P->Topo;
        P->Topo = P->Topo->Prox;
    }
    return Ret;
}

float Calculo_parcial(float num1, float num2, char op[]){
    float retorno;
    if(!strcmp(op, "+")){
        retorno=num1+num2;
    }
    if(!strcmp(op, "-")){
        retorno=num1-num2;
    }
    if(!strcmp(op, "*") || !strcmp(op, "×")){
        retorno=num1*num2;
    }
    if(!strcmp(op, "÷") || !strcmp(op, "/")){
        retorno=num1/num2;
    }
    if(!strcmp(op, "<")){
        retorno=(num1<num2);
    }
    if(!strcmp(op, ">")){
        retorno=(num1>num2);
    }
    if(!strcmp(op, "<=")){
        retorno=(num1<=num2);
    }
    if(!strcmp(op, ">=")){
        retorno=(num1>=num2);
    }
    if(!strcmp(op, "!=")){
        retorno=(num1!=num2);
    }
    if(!strcmp(op, "=")){
        retorno=(num1=num2);
    }
    return retorno;
}

void Resolver_pilha(Pilha *PNum, Pilha *POp){
    if (Pilha_Vazia(PNum) || PNum->Topo->Prox == NULL || Pilha_Vazia(POp)) return;
    Elemento *e2 = Desempilhar(PNum);
    Elemento *e1 = Desempilhar(PNum);
    Elemento *opElem = Desempilhar(POp);
    
    float num2 = e2->Numero;
    float num1 = e1->Numero;
    char op[10];
    strcpy(op, opElem->Operador);
    
    Empilhar(PNum, "#", Calculo_parcial(num1, num2, op));
    
    free(e2);
    free(e1);
    free(opElem);
}

int preced(const char *Op){
    if(strcmp(Op, "(") == 0 || strcmp(Op, ")") == 0) return 0;
    if(strcmp(Op, "<=")==0 || strcmp(Op, ">=")==0 || strcmp(Op, "<")==0 || strcmp(Op, ">")==0 || strcmp(Op, "!=")==0 || strcmp(Op, "==")==0) return 1;
    if(strcmp(Op, "+")==0 || strcmp(Op, "-")==0) return 2;
    if(strcmp(Op, "*")==0 || strcmp(Op, "×")==0 || strcmp(Op, "/")==0 || strcmp(Op, "÷")==0) return 3;
    return 0;
}

float Calcular_exp(Fila *F){
    Pilha PNum, POp;
    Ini_Pilha(&PNum);
    Ini_Pilha(&POp);
    Elemento *pto;
    
    for(pto = F->Inicio; pto != NULL; pto = pto->Prox){
        if(strcmp(pto->Operador, "#") == 0){
            Empilhar(&PNum, "#", pto->Numero);
        }
        else if(strcmp(pto->Operador, "(") == 0){
            Empilhar(&POp, "(", 0);
        }
        else if(strcmp(pto->Operador, ")") == 0){
            // Desempilha até encontrar o '(' correspondente
            while(!Pilha_Vazia(&POp) && strcmp(POp.Topo->Operador, "(") != 0){
                Resolver_pilha(&PNum, &POp);
            }
            // Remove o '(' da pilha de operadores
            if(!Pilha_Vazia(&POp)){
                Elemento *rem = Desempilhar(&POp);
                free(rem);
            }
        }
        else{ // Operadores normais (+, -, *, /, etc.)
            while(!Pilha_Vazia(&POp) && strcmp(POp.Topo->Operador, "(") != 0 && preced(pto->Operador) <= preced(POp.Topo->Operador)){
                Resolver_pilha(&PNum, &POp);
            }
            Empilhar(&POp, pto->Operador, 0);
        }
    }
    
    // Esvazia o restante da pilha de operadores
    while(!Pilha_Vazia(&POp)){
        Resolver_pilha(&PNum, &POp);
    }
    
    float resultado = 0;
    if(!Pilha_Vazia(&PNum)){
        resultado = PNum.Topo->Numero;
    }
    return resultado;
}

void Menu(){
    int Op;
    Fila F;
    Ini_Fila(&F);
    
    do{
        cout << "\n[1] - Inserir expressao\n[2] - Calcular expressao\n[3] - Sair\nOpcao: ";
        cin >> Op;
        switch(Op){
            case 1: 
                Digitar_exp(&F);
                system("cls");
                break;
            case 2: 
                if(Fila_Vazia(&F)){
                    cout << "\nInsira uma expressao primeiro!\n";
                } else {
                    cout << "\nO resultado da expressao e: " << Calcular_exp(&F) << "\n";
                }
                system("pause");
                system("cls");
                break;
            case 3: 
                cout << "\nSaindo...\n";
                break;
            default:
                cout << "\nOpcao invalida\n";
                system("cls");
                break;
        }
    } while(Op != 3);
}

int main(){
    Menu();
    return 0;
}