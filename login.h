#ifndef _LOGIN_
#define _LOGIN_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Structs.h"

int checa_login(User *proximo_ptr2);

void registrar(User *proximo_ptr2) {
    char linha[70];
    int status;
    FILE * fregistro;
    fregistro = fopen("login.txt", "a");

    if(fregistro == NULL) {
        printf("Erro na abertura do arquivo\n");
        exit(1);
    }
    printf("Digite o login: ");
    fgets(proximo_ptr2->login, 50, stdin);
    strtok(proximo_ptr2->login, "\n");
    do {
        status = checa_login(proximo_ptr2); // Checa se o login já existe, a função precisa ser chamada de novo todas as vezes para que o ponteiro volte para o início do arquivo e cheque todas as possibilidades
    }while(status != 0);

    printf("Digite a senha: ");
    fgets(proximo_ptr2->senha, 20, stdin);
    // Login e senha são gravados no arquivo no formato "login:senha" para facilitar a autenticação
    fprintf(fregistro, "%s:%s", proximo_ptr2->login, proximo_ptr2->senha);
    fclose(fregistro);

    proximo_ptr2->proximo = (User *)malloc(sizeof(User)); 
    proximo_ptr2 = proximo_ptr2->proximo;
    proximo_ptr2->proximo = NULL;
}

int autenticar(void) { // Retorna 0 caso dê certo e 1 caso dê errado
    char login[51], senha[21];
    FILE * fleitura;
    fleitura = fopen("login.txt", "r");
    if(fleitura == NULL) {
        printf("Erro na abertura do arquivo\n");
        exit(1);
    }

    char linha[71], cmp[71];
    printf("Digite seu login: ");
    fgets(login, 51, stdin);
    strtok(login, "\n");
    printf("Digite sua senha: ");
    fgets(senha, 21, stdin);
    
    strcpy(cmp, login);
    strcat(cmp, ":"); // Deixa as strings lidas no formato "login:senha" para a comparação
    strcat(cmp, senha);

    while(!feof(fleitura)) { 
        fgets(linha, 71, fleitura);
        if(strcmp(cmp, linha) == 0) {
            printf("Login realizado com sucesso!\n");
            fclose(fleitura); 
            return 0;
        }
        if(feof(fleitura)) {
            printf("O login não foi realizado com sucesso\n");
            fclose(fleitura); 
            return 1;
        }
    }
    printf("O login não foi realizado com sucesso\n"); // Caso não entre no loop
    fclose(fleitura); 
    return 1;
}

int checa_login(User *proximo_ptr2) {
    char linha[70];
    FILE * fcheca_login;
    fcheca_login = fopen("login.txt", "r");
    while(!feof(fcheca_login)) {
        fgets(linha, 70, fcheca_login);
        strtok(linha, ":"); // Corta a string antes do ":", selecionando só o login
        if(strcmp(linha, proximo_ptr2->login) == 0) {
            do {
                printf("Erro: login já existe!\n");
                printf("Tente outro login: ");
                fgets(proximo_ptr2->login, 50, stdin);
                strtok(proximo_ptr2->login, "\n");
            }while(strcmp(linha, proximo_ptr2->login) == 0);
            fclose(fcheca_login);
            return 1;
        }
    }
    fclose(fcheca_login);
    return 0;
}

#endif //_LOGIN_