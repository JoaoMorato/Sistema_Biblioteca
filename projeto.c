#include <stdio.h>
#include <stdlib.h>
#include "Structs.h"
#include "login.h"
#include "biblioteca.h"

#ifdef __WIN32__
#include <windows.h>
  void SetCursorPosition (short x, short y){
    COORD Coord;
    Coord.X = XPos;
    Coord.Y = YPos;
  
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Coord);
  }
#else
  void SetCursorPosition (short x, short y){
    printf("\033[%d;%dH", y, x);
  }
#endif

void ClearEndLine(){
  while(getchar() != '\n');
}

void ShowLivro(Livro livro){
  system("clear");
  printf("Nome: %s\n", livro.titulo);
  printf("Autor: %llu\n", livro.autor);
  printf("Cod: %llu\n", livro.codigo);
  getchar();
}

void adc_liv(Livro *proximo_ptr1){
  
  system("clear");
  printf("NOVO LIVRO\n--------------------\n");
  printf("Insira seu título: ");
  scanf("%40[^\n]", proximo_ptr1->titulo);
  proximo_ptr1->autor = 0;
  proximo_ptr1->titulo[40] = 0;
  if(!Add_Livro(proximo_ptr1)) printf("Falha ao Adicionar Livro!");
  else{
    proximo_ptr1->proximo = (Livro *)malloc(sizeof(Livro));
    proximo_ptr1 = proximo_ptr1->proximo;
    proximo_ptr1->proximo = NULL;
  }
}

void MostrarBiblioteca(){
  int resp = 0;
  int pagina = 0;
  int totalPag;
  do{
    system("clear");
    Livro* livros = ShowBiblioteca(pagina, &totalPag, 20);
    if(!livros){
      printf("Erro: Nenhum livro encontrado.\n");
      getchar();
      break;
    }
    SetCursorPosition(1, 2);
    for(int i = 0; i<20; i++){
      if(!livros[i].codigo) break;
      printf("%d - %s\n", i+1, livros[i].titulo);
    }
    SetCursorPosition(1,1);
    printf("Pagina %d <1 - %d>(0-Voltar): ", pagina+1, totalPag);
    scanf("%d", &resp);
    if(resp == '+' && pagina+1 < totalPag) pagina++;
    else if(resp == '-' && pagina+1 > 1) pagina--;
    else if(resp >= 1 && resp <= 20) {
      if(livros[resp-1].codigo){
        ShowLivro(livros[resp]);
      }
    }
  }while(resp);
}

void Procurar(Livro *inicio_ptr1, Livro *proximo_ptr1){
  system("clear");
  char buff[41];
  printf("Digite o nome do Livro: ");
  scanf("%40[^\n]", buff);
  ClearEndLine();
  Livro* livros = Buscar_Livro(buff);
  if(!livros[0].codigo){
    printf("Livro não encontrado.");
  }else{
    short numLivros = 0;
    while(livros[numLivros].codigo) numLivros++;
    char numPag = (numLivros/20) + (numLivros % 20 ? 1 : 0);
    char pagina = 0;
    while(1){
      SetCursorPosition(1, 3);
      for(int i = 0; i<20; i++){
        if(!livros[i+(pagina*20)].codigo) break;
        printf("\aLivro: %s\n", livros[i+(pagina*20)].titulo);
        printf("Cod: %llu\n", livros[i+(pagina*20)].codigo);
      }
      SetCursorPosition(1, 2);
      printf("Pagina %i <1 - %i>(0 - Sair): ", pagina+1, numPag);
      int resp;
      scanf("%d", &resp);
      pagina = resp > numPag ? numPag - 1 : resp;
      if(!resp) break;
      getchar();
    }
  }
  getchar();
}

void ver_liv(Livro *inicio_ptr1, Livro *proximo_ptr1){
    system("clear");
    proximo_ptr1 = inicio_ptr1;
    
    while(proximo_ptr1 != NULL) {
        printf("Livro - %s\n", proximo_ptr1->titulo);
        proximo_ptr1 = proximo_ptr1->proximo;
    }
}

void menu_cliente(Livro *inicio_ptr1, Livro *proximo_ptr1){
  int opc;
  char fim = 0;
  while(!fim){
    do {
      system("clear");        
      printf("MENU\n--------------------");
      printf("1 - Biblioteca\n");
      printf("2 - Buscar Livro\n");
      printf("0 - Sair!\n\n");
      printf("Sua opção é: ");
      scanf("%i", &opc);
    
    } while (opc != 1 && opc != 0);

    switch (opc){
      case 1:
        //ver_liv(inicio_ptr1, proximo_ptr1);
        break;
      case 2:
        Procurar(inicio_ptr1, proximo_ptr1);
        break;
      case 0:
        printf("\nObrigado por usar nosso sistema, até mais!\n");
        fim = 1;
        break;
    }
  }
}
void menu_admin(Livro *proximo_ptr1, Livro *inicio_ptr1){
  int opc;
  char fim = 0;
  while(!fim){
    do {
      system("clear");        
      printf("MENU\n--------------------\n");
      printf("1 - Adicionar livro\n");
      printf("2 - Remover livro\n");
      printf("3 - Buscar Livro\n");
      printf("0 - Sair!\n\n");
      printf("Sua opção é: ");
      scanf("%i", &opc);
    
    } while (opc < 0 || opc > 3);
    ClearEndLine();
    switch (opc) {
      case 1:
        adc_liv(proximo_ptr1);
        break;
      case 2:
        //rem_liv(conta);
        break;
      case 3:
        Procurar(inicio_ptr1, proximo_ptr1);
        break;
      case 0:
        printf("\nObrigado por usar nosso sistema, até mais!\n");
        fim = 1;
        break;
    }
  }
}

int main(void) {
  Livro * inicio_ptr1;
  Livro * proximo_ptr1;
  User * inicio_ptr2;
  User * proximo_ptr2;

  inicio_ptr1 = (Livro *)malloc(sizeof(Livro));
    if (inicio_ptr1 == NULL)
        exit(1);
    proximo_ptr1 = inicio_ptr1;

  inicio_ptr2 = (User *)malloc(sizeof(User));
    if (inicio_ptr2 == NULL)
        exit(1);
    proximo_ptr2 = inicio_ptr2;
    
  int opc = 0;
  char fim = 0;
  while(!fim){
    do {
      system("clear");
      if (opc > 2){
          printf("\n\nSelecione uma opção válida!!!\n\n");
      }
      printf("MENU\n--------------------\n");
      printf("1 - Cadastro\n");
      printf("2 - Login\n");
      printf("0 - Sair!\n\n");
      printf("Sua opção é: ");
      scanf("%i", &opc);
    
    } while (opc != 1 && opc != 2 && opc != 0);
    ClearEndLine();
    switch (opc){
      case 1:
        registrar(proximo_ptr2);
        break;
      case 2:
        if(!autenticar())
        menu_admin(proximo_ptr1, inicio_ptr1);
        break;
      case 0:
        fim = 1;
        break;
    }   
  }   
    return 0;
}