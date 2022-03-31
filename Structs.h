#ifndef _STRUCTS_
#define _STRUCTS_

typedef struct {
    char login[30];
    char senha[20];
    unsigned long long livros[5];
    char admin;
    struct Node *proximo;
}User;

typedef struct{
    char nome[51];
    char biografia[501];
    unsigned long long cod;
    unsigned long long livros_escritos[999];
    struct Node *proximo;
}Autor;

typedef struct{
    unsigned long long codigo;
    char titulo[41];
    unsigned long long autor;
    struct Node *proximo;
}Livro;

#endif