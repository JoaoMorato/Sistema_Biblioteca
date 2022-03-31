#ifndef _BIBLIOTECA_
#define _BIBLIOTECA_

#include "Structs.h"
#include <stdio.h>
#include <stdlib.h>

#define ull unsigned long long

void Upper_Case(char* frase){
  size_t cont = 0;
  while(frase[cont]){
    if(frase[cont] >= 'a' && frase[cont] <= 'z'){
      char aux = frase[cont] - 'a';
      frase[cont] = 'A' + aux;
    }
    cont++;
  }
}

Livro* ShowBiblioteca(int pagina, int *totalPagina, int livrosPagina){
  Livro* livro = (Livro*)malloc(sizeof(Livro)*livrosPagina);
  livro[0].codigo = 0;
  FILE* arq = fopen("livros.txt", "rb");
  if(!arq){
    printf("Falha ao abrir 'livros.txt'.\n");
    getchar();
    return NULL;
  }
  ull quant;
  fread(&quant, sizeof(ull), 1, arq);
  *totalPagina = quant / livrosPagina + (quant % livrosPagina ? 1 : 0);
  char quantLivros = 0;
  int resp;
  ull move = pagina * 56 * livrosPagina;
  if(fseek(arq, move, SEEK_CUR)){
    printf("Pagina nao encontrada.\n");
    getchar();
    fclose(arq);
    return NULL;
  }
  
  while(fread(&livro[quantLivros].codigo, 8, 1, arq) != EOF){
    fread(livro[quantLivros].titulo, 1, 40, arq);
    livro[quantLivros].titulo[40] = 0;
    fread(&livro[quantLivros].autor, 8, 1, arq);
    quantLivros++;
    if(quantLivros >= 20) break;
    livro[quantLivros].codigo = 0;
  }
  fclose(arq);
  return livro;
}

Livro* Buscar_Livro(char* nome){
  Upper_Case(nome);
  Livro *livro = (Livro*)malloc(sizeof(Livro)*999);
  livro[0].codigo = 0;
  FILE* arq = fopen("livros.txt", "rb");
  if(!arq){
    printf("\nFalha ao abrir arquivo 'livros.txt'.\n");
    return livro;
  }
  ull quant;
  fread(&quant, sizeof(ull), 1, arq);
  if(feof(arq) || ferror(arq)){
    fclose(arq);
    return livro;
  }
  ull _cod = 0;
  int contLivro = 0;
  while(fread(&_cod, sizeof(ull), 1, arq) != EOF){
    if(feof(arq) || ferror(arq)){
      break;
    }
    if(fread(livro[contLivro].titulo, 1, 40, arq) != 40){
      printf("Erro na leitura de arquivo (titulo), 'livros.txt'.");
      break;
    }
    
    char igual = 0;
    char *buffer = livro[contLivro].titulo;
    buffer[40] = 0;
    for(int i = 0; i<41; i++){
      if(!buffer[i]) break;
      if(buffer[i] != nome[0]) continue;
      igual = 1;
      for(size_t j = 1; j<41; j++){
        if(!nome[j]) break;
        i++;
        if(!buffer[i]){
          if(nome[j]) igual = 0;
          break;
        }
        if(buffer[i] == nome[j]) continue;
        igual = 0;
        break;
      }
      if(igual) break;
    }
    
    
    if(fread(&livro[contLivro].autor, sizeof(ull), 1, arq) != 1){
      printf("Erro na leitura de arquivo(autor), 'livros.txt'.");
      break;
    }
    livro[contLivro].codigo = _cod;
    if(igual)
      contLivro++;
    if(contLivro>=20)
      break;
    livro[contLivro].codigo = 0;
  }
  fclose(arq);
  return livro;
}

int Add_Livro(Livro *proximo_ptr1){
  Upper_Case(proximo_ptr1->titulo);
  FILE* arq = fopen("livros.txt", "r+b");
  if(!arq){
    printf("\nFalha ao abrir arquivo 'livros.txt'.\n");
    return 0;
  }
  ull quant = 0;
  fread(&quant, sizeof(ull), 1, arq);
  ++quant;
  
  rewind(arq);
  fwrite(&quant, sizeof(ull), 1, arq);
  
  freopen("livros.txt", "ab", arq);
  fwrite(&quant, sizeof(ull), 1, arq);
  fwrite(proximo_ptr1->titulo, 1, 40, arq);
  fwrite(&proximo_ptr1->autor, sizeof(ull), 1, arq);
  fclose(arq); 
    
  return 1;
}

#endif //_BIBLIOTECA_