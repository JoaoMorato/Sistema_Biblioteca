#define _GNU_SOURCE
#define __USE_BSD /* usleep() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "Animacoes/Funcoes_Anin/desenharLivro.h"
#include "Animacoes/Funcoes_Anin/term.h"
#include "Funcoes/aluguel.h"
#include "Funcoes/biblioteca.h"
#include "Funcoes/crypto.h"
#include "Funcoes/login.h"
#include "Funcoes/pagamento.h"
#include "Listas/listaAutor.h"
#include "Listas/listaLivro.h"
#include "Listas/structs.h"

#if defined __USE_BSD || defined __USE_XOPEN_EXTENDED

extern __useconds_t ualarm(__useconds_t __value, __useconds_t __interval) __THROW;

extern int usleep(__useconds_t __useconds);
#endif

//-----------------------MENUS-------------------------

void MenuAdmin ();
void MenuCliente ();
void MenuPagamento ();

//-------------------FUNCIONALIDADES--------------------

User* Login (char, char);
void AninLoading ();
void MostrarAutor (ull);
void AdicionarLivro ();
void ProcurarLivro ();
void ProcurarAutor ();
void MostrarBiblioteca ();

User *usuario = NULL;

//------------------------MAIN--------------------------

int main (void) {
	char resp;
	int opc = 0;
	int status;
	char fim = 0;
	char *ops[] = {"CADASTRO", "LOGIN", "LOGIN ADMIN", "SAIR"};

	tc_echo_off();
	tc_canon_off();
	HideCursor();

	AnimLivro('A');

	while (!fim) {

		if (usuario)
			free(usuario);
		UnsaveText(1);

		do {
			SetCursorPosition(1, 1);
			DrawBegin();
			DrawTextLeft("MENU PRINCIPAL", TC_GRN);
			DrawTextSpace('l');

			for (int i = 0; i < 4; i++) {
				if (opc == i)
					DrawTextLeft(ops[i], TC_RED);
				else
					DrawTextLeft(ops[i], TC_GRN);
			}
			SaveText('l');
			DrawEnd();

			resp = getchar();

			if (resp == 27) {
				if (!kbhit()) {
					opc = 3;
					break;
				}
				getchar();
				resp = getchar();
				if (resp == 'A' && opc > 0)
					opc--;
				else
					if (resp == 'B' && opc < 3)
						opc++;
			}
		} while (resp != 10);

		switch (opc) {
			case 0:
				usuario = Login(0, 1);
				break;
			case 1:
				usuario = Login(0, 0);
				if (usuario) {
					AnimLivro('D');
					AninLoading();
					AnimLivro('D');

					MenuCliente(usuario);
				}
				break;
			case 2:
				usuario = Login(1, 0);
				if (usuario) {
					AnimLivro('D');
					AninLoading();
					AnimLivro('D');

					MenuAdmin(usuario);
				}
				break;
			case 3:
				AnimLivro('F');
				SetCursorPosition(1, 27);
				fim = 1;
				break;
		}
	}
	return 0;
}

//-----------------------FUNÇÕES------------------------

User* Login (char admin, char reg) {
	char resp = 0;
	char opc = 0;
	char tamL = 0;
	char tamS = 0;
	char error = 10;
	char senha[40] = {0};
	
	User* novoUsuario = (User*)malloc(sizeof(User));

	if (!novoUsuario)
		exit(15);

	novoUsuario->admin = admin;
	memset(novoUsuario->senha, 0, 41);
	memset(novoUsuario->login, 0, 30);

	while (error) {

		DrawBegin();
		DrawTextSpace('r');
		DrawTextSpace('r');

		if (opc && reg != 2)
			DrawTextRight("LOGIN:", TC_GRN);
		else
			DrawTextRight("LOGIN:", TC_RED);

		if (!opc && reg != 2)
			DrawTextRight(novoUsuario->login, TC_BLK);
		else
			DrawTextRight(novoUsuario->login, TC_GRN);

		DrawTextSpace('r');

		if (!opc && reg != 2)
			DrawTextRight("SENHA:", TC_GRN);
		else
			if (reg != 2)
				DrawTextRight("SENHA:", TC_RED);

		if (!opc && reg != 2)
			DrawTextRight(senha, TC_GRN);
		else
			if (reg != 2)
				DrawTextRight(senha, TC_BLK);

		if (error == 2 && reg == 1) {
			DrawTextSpace('r');
			DrawTextSpace('r');
			DrawTextRight("Login existente!", TC_YEL);
		}
		else {
			if (error == 1 && !reg) {
				DrawTextSpace('r');
				DrawTextSpace('r');
				DrawTextRight("Login invalido!", TC_YEL);
			}
		}
		LoadText();
		DrawEnd();

		resp = getchar();

		switch (resp) {
			case 9:
				if (reg != 2) opc = !opc;
			break;
			case 10:
				if (reg != 2)
					opc = !opc;
				if (!tamS && reg != 2) {
					resp = 0;
					break;
				}
				switch (reg) {
					case 0:
						error = Autenticar(novoUsuario);
					break;
					case 1:
						error = Registrar(novoUsuario);
					break;
					case 2:
						error = VirarAdmin(novoUsuario);
					break;
					case 3:
						error = CriarAdmin(novoUsuario);
					break;
				}
			break;
			case 27:
				if(!kbhit()) {
					free(novoUsuario);
					return NULL;
				}
				getchar();
				getchar();
			break;
			case 8:
			case 127:
				if (!opc && tamL > 0)
					novoUsuario->login[--tamL] = 0;
				if (opc && tamS > 0) {
					novoUsuario->senha[--tamS] = 0;
					senha[tamS] = 0;
				}
			break;
			default:
				if ((resp >= 'a' && resp <= 'z') || (resp >= 'A' && resp <= 'Z') || (resp >= '0' && resp <= '9')) {
					if (!opc && tamL < 29)
						novoUsuario->login[tamL++] = resp;
					if (opc && tamS < 19) {
						senha[tamS] = '*';
						novoUsuario->senha[tamS++] = resp;
					}
				}
			break;
		}
	}
	return novoUsuario;
}

void AninLoading () {
	char num[5] = {0};
	char barra[13] = "[          ]\0";
	
	for (int i = 0; i <= 100; i++) {
		DrawBegin();

		DrawTextSpace('l');
		DrawTextLeft("Carregando...", TC_BLK);

		if (i / 10){
			barra[(int)(i/10)] = '#';
		}
		NumToStr(i, num, 10);
		DrawTextLeft(num, TC_BLK);

		if (i / 10 < 5)
			DrawTextLeft((char *)barra, TC_RED);
		else
			if (i / 10 < 8)
				DrawTextLeft((char *)barra, TC_YEL);
		else
			DrawTextLeft((char *)barra, TC_GRN);
		SaveText('l');
		DrawEnd();
		usleep(50000);
	}
	
	UnsaveText('r');
	DrawBegin();
	LoadText();

	DrawTextSpace('l');
	DrawTextSpace('l');
	DrawTextLeft("Dados carregados!", TC_BLK);

	DrawEnd();
	sleep(3);
}

void MenuCliente (User *usuario) {
	char resp;
	char fim = 0;
	int opc = 0;
	int status;
	int cont = -1;
	char *ops[] = {"Biblioteca", "Procurar Livro", "Procurar Autor", "Devolver Livro",	"Renovar Livro", "Meus Livros", "Menu Pagamento", "Sair"};

	do {
		cont++;
		status = ChecaDia(usuario);
	} while (!status);

	if (cont) {
		ClearScreen();
		printf("Nenhum livro a mais foi encontrado fora do prazo!\n");
		printf("\nEntrando no Menu...\n");
		sleep(4);
	}
	while (!fim) {

		do {
			DrawBegin();
			DrawTextLeft("MENU", TC_GRN);
			DrawTextSpace('l');

			for (int i = 0; i < 8; i++) {
				if (opc == i)
					DrawTextLeft(ops[i], TC_RED);
				else
					DrawTextLeft(ops[i], TC_GRN);
			}

			SaveText('l');
			DrawEnd();
			resp = getchar();

			if (resp == 27) {
				if (!kbhit()) {
					opc = 7;
					break;
				}
				getchar();
				resp = getchar();
				if (resp == 'A' && opc > 0)
					opc--;
				else if (resp == 'B' && opc < 7)
					opc++;
			}
		} while (resp != 10);

		switch (opc) {
			case 0:
				AnimLivro('d');
				MostrarBiblioteca();
				break;
			case 1:
				ProcurarLivro();
				break;
			case 2:
				ProcurarAutor();
				break;
			case 3:
				MeusLivros(usuario, 'D');
				break;
			case 4:
				MeusLivros(usuario, 'R');
				break;
			case 5:
				MeusLivros(usuario, 'O');
				break;
			case 6:
				AnimLivro('D');
				MenuPagamento(usuario);
				break;
			case 7:
				AnimLivro('E');
				fim = 1;
				break;
		}
	}
}

void MenuPagamento (User *usuario) {
	char resp;
	char fim = 0;
	int opc = 0;
	int status;
	int cont = -1;
	char *ops[] = {"Adicionar Cartao",	"Remover Cartao", "Meus Cartoes", "Sair"};

	while (!fim) {

		tc_canon_off();
		tc_echo_off();
		HideCursor();

		do {
			DrawBegin();
			DrawTextLeft("MENU", TC_GRN);
			DrawTextSpace('l');

			for (int i = 0; i < 4; i++) {
				if (opc == i)
					DrawTextLeft(ops[i], TC_RED);
				else
					DrawTextLeft(ops[i], TC_GRN);
			}
			DrawEnd();
			resp = getchar();

			if (resp == 27) {
				if (!kbhit()) {
					opc = 3;
					break;
				}
				getchar();
				resp = getchar();
				if (resp == 'A' && opc > 0)
					opc--;
				else if (resp == 'B' && opc < 3)
					opc++;
			}
		} while (resp != 10);

		switch (opc) {
			case 0:
				AdicionarCartao(usuario, 'A');
				break;
			case 1:
				MeusCartoes(usuario, 'D');
				break;
			case 2:
				MeusCartoes(usuario, 'O');
				break;
			case 3:
				AnimLivro('E');
				fim = 1;
				break;
		}
	}
}

void MenuAdmin (User *usuario) {
	char resp;
	char fim = 0;
	int opc = 0;
	char *ops[] = {"Adicionar Livro", "Procurar Livro", "Procurar Autor", "Tornar Admin", "Criar Admin", "Biblioteca", "Ver Fundos",	"Sair"};

	User* novoUsuario = (User*)malloc(sizeof(User));
	
	while (!fim) {

		do {
			DrawBegin();
			DrawTextLeft("MENU", TC_GRN);
			DrawTextSpace('l');

			for (int i = 0; i < 8; i++) {
				if (opc == i)
					DrawTextLeft(ops[i], TC_RED);
				else
				DrawTextLeft(ops[i], TC_GRN);
			}
			SaveText('l');
			DrawEnd();

			resp = getchar();

			if (resp == 27) {
				if (!kbhit()) {
					opc = 7;
					break;
				}
				getchar();
				resp = getchar();
				if (resp == 'A' && opc > 0)
					opc--;
				else
					if (resp == 'B' && opc < 7)
						opc++;
			}
		} while (resp != 10);

		switch (opc) {
			case 0:
				AdicionarLivro();
			break;
			case 1:
				UnsaveText('r');
				ProcurarLivro();
			break;
			case 2:
				UnsaveText('r');
				ProcurarAutor();
			break;
			case 3:
				novoUsuario = Login(0, 2);
			break;
			case 4:
				novoUsuario = Login(0, 3);
			break;
			case 5:
				AnimLivro('d');
				MostrarBiblioteca();
			break;
			case 6:
				VerFundos();
			break;
			case 7:
				AnimLivro('E');
				fim = 1;
			break;
		}
	}
}

void EditarLivro(Livro livro){
	char resp = 0;
	char posY = 0;
	char posX = 0;
	Livro antigo = livro;
	
	char quant[5] = {0};
	NumToStr(livro.quantidade, quant, 10);
	
	char preco[11] = {0};
	preco[0] = 'R';
	preco[1] = '$';
	gcvt(livro.preco, 6, &preco[2]);
	char posiP = strlen(preco) - 2;
	if(posiP && preco[2] == '0') posiP = 0;
	char posiPonto = 0;
	char ponto = 0;
	for(int i = 0; preco[i]; i++){
		if(preco[i] != '.') continue;
		ponto = 1;
		posiPonto = i - 2;
		break;
	}
	
	char erro = -1;
	char enter = 0;
	char* escolha;
	
	while(1){
		if(!enter)
			escolha = TC_RED;
		else escolha = TC_BLK;
		
		DrawBegin();
		
		DrawTextSpace('l');
		DrawTextLeft("Titulo:", TC_GRN);
		DrawTextLeft(livro.titulo,  TC_GRN);
		
		DrawTextSpace('l');
		DrawTextLeft("Quantidade:", TC_GRN);
		DrawTextLeft(quant, !posY && !posX ? escolha : TC_GRN);
		
		DrawTextSpace('l');
		DrawTextLeft("Preco:", TC_GRN);
		DrawTextLeft(preco, posY && !posX ? escolha : TC_GRN);

		DrawTextSpace('r');
		DrawTextSpace('r');
		DrawTextSpace('r');
		DrawTextRight("[SALVAR]", !posY && posX ? TC_RED : TC_GRN);
		DrawTextSpace('r');
		DrawTextSpace('r');
		DrawTextRight("[VOLTAR]", posY && posX ? TC_RED : TC_GRN);
		if(erro > 0){
			DrawTextSpace('r');
			DrawTextRight("Falha ao salvar.", TC_YEL);
		}else if(!erro){
			DrawTextSpace('r');
			DrawTextRight("Livro atualizado.", TC_YEL);
		}
		
		DrawEnd();

		resp = getchar();

		switch(resp){
			case 10:
				if(!posX) { enter = !enter; break; }
				if(posY){
					AnimLivro('e');
					return;
				}
				livro.preco = atof(&preco[2]);
				erro = AttLivro(livro);
			break;
			case 27:
				if(!kbhit()){
					if(enter){
						enter = 0;
						break;
					}
					AnimLivro('e');
					return;
				}
				if(getchar() != '[') { getchar(); break; }
				resp = getchar();
				switch(resp){
					case 'A':
						if(posY && !enter) posY--;
					break;
					case 'B':
						if(posY || enter) break;
						posY++;
					break;
					case 'C':
						if(enter){
							if(!posY){
								livro.quantidade++;
								NumToStr(livro.quantidade, quant, 10);
							}
							break;
						}
						if(posX) break;
						posX++;
						posY = 0;
					break;
					case 'D':
						if(enter){
							if(!posY && (livro.quantidade - livro.alugado)){
								livro.quantidade--;
								NumToStr(livro.quantidade, quant, 10);
							}
							break;
						}
						if(!posX) break;
						posX--;
						posY = 0;
					break;
					default:
						enter = 0;
					break;
				}
			break;
			case 8:
			case 127:
				if(!posY) break;
				
				if(!posiP) break;
				if(preco[--posiP+2] == '.') ponto = 0;
				preco[posiP+2] = 0;
				if(!posiP) preco[posiP+2] = '0';
			break;
			default:
				if(!enter || !posY || posiP >= 8) break;
				if(resp == ',') resp = '.';
				if((resp < '0' || resp > '9') && resp != '.') break;

				if(resp == '.' && ponto) break;
				if(ponto && (posiP - posiPonto) > 2) break;
				if(resp == '.') { ponto = 1; posiPonto = posiP; }
				
				preco[2 + posiP] = resp;
				posiP++;
			break;
		}
	}
}

void MostrarLivro (Livro livro) {
	Autor autor;
	BuscarAutorCod(livro.autor, &autor);
	
	char preco[13] = {0};
	preco[0] = 'R';
	preco[1] = '$';
	gcvt(livro.preco, 10, &preco[2]);
	
	char ponto = 0, count = 0;
	for(int i = 0; i<10; i++){
		if(!preco[i]) break;
		if(preco[i] == '.') ponto = 1;
		else if(ponto) count++;
		if(count == 3){
			preco[i] = 0;
			break;
		}
	}
	
	char resp = 0;
	char posY = 0;

	char check = ChecaAluguel(usuario, livro.codigo);

	while(1){
		DrawBegin();
		DrawTextSpace('l');
		DrawTextSpace('l');
		DrawTextLeft("Nome:", TC_GRN);
		DrawTextLeft(livro.titulo, TC_BLK);
		DrawTextSpace('l');
		DrawTextLeft("Autor:", TC_GRN);
		DrawTextLeft(autor.nome, TC_BLK);
		DrawTextSpace('l');
		DrawTextLeft("Preco:", TC_GRN);
		DrawTextLeft(preco, TC_BLK);

		DrawTextSpace('r');
		DrawTextSpace('r');
		DrawTextSpace('r');

		if(!usuario->admin){
			if(livro.quantidade - livro.alugado && check == 1)
				DrawTextRight("[ALUGAR]", !posY ? TC_RED : TC_GRN);
			else { DrawTextRight("[INDISPONIVEL]", TC_BLU); posY = 1; }
		}else DrawTextRight("[EDITAR]", !posY ? TC_RED : TC_GRN);
		
		DrawTextSpace('r');
		DrawTextSpace('r');
		DrawTextRight("[VOLTAR]", posY ? TC_RED : TC_GRN);
		
		DrawEnd();
		resp = getchar();

		switch(resp){
			case 27:
				if(!kbhit()){
					AnimLivro('e');
					return;
				}
				if(getchar() != '[') { getchar(); break; }
				resp = getchar();
				switch(resp){
					case 'A':
						if(!posY || (livro.quantidade == livro.alugado && !usuario->admin)) break;
						posY--;
					break;
					case 'B':
						if(!posY) posY++;
					break;
				}
			break;
			case 10:
				if(posY) {
					AnimLivro('e');
					return;
				}
				if(!usuario->admin){
					AlugarLivro(usuario, livro.codigo);
					check = ChecaAluguel(usuario, livro.codigo);
				} else {
					AnimLivro('d');
					EditarLivro(livro);
					livro = BuscarLivroCod(livro.codigo);
					gcvt(livro.preco, 10, &preco[2]);
				}
			break;
		}
	}
}

void AdicionarLivro () {
	char resp;
	Livro livro;
	livro.codigo = 0;
	livro.preco = 0;
	memset(livro.titulo, 0, 51);
	char preco[9] = {0};
	preco[0] = 'R';
	preco[1] = '$';
	preco[2] = '0';
	
	Autor autor;
	memset(autor.nome, 0, 51);
	autor.cod = 0;
	
	char tab = 0;
	char posiL = 0;
	char posiA = 0;
	char posiP = 0;
	char erro = 0;
	char ponto = 0;
	

	while (resp != 27){
		SetCursorPosition(1,1);
		DrawBegin();

		DrawTextSpace('r');
		DrawTextRight("Titulo:", !tab ? TC_RED : TC_GRN);
		DrawTextRight(livro.titulo, TC_BLK);

		DrawTextSpace('r');
		DrawTextRight("Autor:", tab == 1 ? TC_RED : TC_GRN);
		DrawTextRight(autor.nome, TC_BLK);

		DrawTextSpace('r');
		DrawTextRight("Preco:", tab == 2 ? TC_RED : TC_GRN);
		DrawTextRight(preco, TC_BLK);

		LoadText();

		DrawEnd();

		resp = getchar();

		switch (resp) {
			case 10:
				if (!livro.titulo[0] || !autor.nome[0]) {
					tab = !tab;
					break;
				}
				listaAutor* listaA = InitListA(listaA);
				BuscarAutor(listaA, autor.nome, 1);
				
				if (listaA->size) autor = listaA->inicio->autor;
				if (!autor.cod) autor = Add_Autor(autor);

				
				livro.autor = autor.cod;
				livro.preco = atof(&preco[2]);
				Add_Livro(&livro);
				
				Add_LivroAutor(autor.cod, livro.codigo);
				
				DeleteListA(listaA);
				return;
			break;
			case 27:
				if (!kbhit()) return;
				if(getchar() != '[') { getchar(); break; }
				resp = getchar();
				switch(resp){
					case 'A':
						if(tab) tab--;
					break;
					case 'B':
						if(tab < 2) tab++;
					break;
				}
			break;
			case 8:
			case 127:
				if (!tab && posiL)
					livro.titulo[--posiL] = 0;
				else if (tab == 1 && posiA)
					autor.nome[--posiA] = 0;
				else if(tab == 2 && posiP){
					posiP--;
					if(posiP == ponto) ponto = 0;
					preco[posiP + 2] = 0;
					if(!posiP) preco[2] = '0';
				}
			break;
			default:
				if (resp >= 32 && resp <= 126){
					if (resp >= 'a' && resp <= 'z')
						resp = (resp - 'a') + 'A';
					if (!tab && posiL < 49)
						livro.titulo[posiL++] = resp;
					else if (tab == 1 && posiA < 49)
						autor.nome[posiA++] = resp;
					else if(tab == 2 && posiP < 6){
						if((resp < '0' || resp > '9') 
							&& resp != '.' && resp != ',') break;
						if((resp == '.' || resp == ',') && ponto) break;
						if(resp == '.' || resp == ',') {
							if(!posiP) posiP++;
							ponto = posiP;
						}
						if(ponto && (posiP - ponto) > 2) break;
						if(resp == ',') resp = '.';
						preco[posiP + 2] = resp;
						posiP++;
					}
				}
			break;
		}
	}
}

void ProcurarLivro () {

	size_t pagina = 0;
	size_t quant = 0;

	char livroPorPagina = 5;
	char buff[51] = {0};
	char posiY = 0;
	char resp = 0;
	listaLivro *livros = InitList(livros);

	while (resp != 27) {

		DrawBegin();
		DrawTextRight("Titulo:", TC_GRN);
		DrawTextRight(buff, TC_BLK);

		if (livros) {
			char count = 0;
			DrawTextSpace('r');
			for (t_noL *no = livros->inicio; no != livros->fim; no = no->proximo) {
				if (count == posiY)
					DrawTextRight(no->livro.titulo, TC_RED);
				else
					DrawTextRight(no->livro.titulo, TC_GRN);
				count++;
			}
		}
		LoadText();
		DrawEnd();

		resp = getchar();
		switch (resp) {
			case 27:
				if (!kbhit())
					break;
				getchar();
				resp = getchar();

				switch (resp) {
					case 'A':
						if (!posiY)
							break;
						posiY--;
						break;
					case 'B':
						if (posiY + 1 >= livros->size)
							break;
						posiY++;
						break;
				}
				break;
			case 10:
				{
					t_noL *no = livros->inicio;
					int count = 0;

					while (no != livros->fim) {
						if (count == posiY)
							break;
						count++;
						no = no->proximo;
					}
					if (no == livros->fim)
						break;
					AnimLivro('d');
					MostrarLivro(no->livro);
				}
				break;
			case 8:
			case 127:
				if (!quant)
					break;
				quant--;
				buff[quant] = 0;
				ClearList(livros);
				posiY = 0;
				if (quant >= 3)
					Buscar_Livro(livros, buff, 3);
				break;
			default:
				if (resp < 32 || resp > 126 || quant > 49)
					break;
				buff[quant++] = resp;
				Upper_Case(buff);
				posiY = 0;
				if (quant >= 3) {
					ClearList(livros);
					Buscar_Livro(livros, buff, 10);
				}
				break;
		}
	}
}

void MostrarAutor (ull autorCod) {
	Autor autor;
	BuscarAutorCod(autorCod, &autor);
	Livro livro;
	char resp = 0;
	size_t posL = 0;
	char posY = 0;
	char max = 0;
	char fim = 0;
	
	while(1){
		DrawBegin();
		if(!posL) LoadText();
		else {
			DrawTextLeft(autor.nome, TC_GRN);
			DrawTextSpace('l');
		}
		
		DrawTextRight(autor.nome, TC_GRN);
		DrawTextSpace('r');

		char aux = 0;
		for(max = 0; max < 20; max++){
			if(!posL && max == 10) break; 
			if(!autor.livros_escritos[max + posL]){
				fim = 1;
				if(max < 10 && posL) DrawTextSpace('l');
				else DrawTextSpace('r');
				continue;
			}
			aux = max;
			livro = BuscarLivroCod(autor.livros_escritos[max + posL]);
			if(max < 10 && posL)
				DrawTextLeft(livro.titulo, posY == max ? TC_RED : TC_GRN);
			else DrawTextRight(livro.titulo, posY == max ? TC_RED : TC_GRN);
		}
		max = aux;
		if(posL < 90 && !autor.livros_escritos[posL + max + 1]) fim = 1;
		
		DrawTextSpace(0);
		if(!fim) DrawTextRight("→", TC_CYN);
		if(posL) DrawTextLeft("←", TC_CYN);
		DrawEnd();

		resp = getchar();

		switch(resp){
			case 10:
				AnimLivro('d');
				MostrarLivro(BuscarLivroCod(autor.livros_escritos[posY + posL]));
			break;
			case 27:
				if(!kbhit()) return;
				getchar();
				resp = getchar();
				switch(resp){
					case 'A':
						if(posY) posY--;
					break;
					case 'B':
						if(posY < max) posY++;
					break;
					case 'C':
						if(posL == 90 || fim) break;
						if(!autor.livros_escritos[posL + 10]) break;
						if(!posL) posL += 10;
						else posL += 20;
						posY = 0;
						AnimLivro('d');
					break;
					case 'D':
						if(!posL) break;
						if(posL == 10) posL -= 10;
						else posL -= 20;
						fim = 0;
						posY = 0;
						AnimLivro('e');
					break;
				}
			break;
		}
	}
}

void ProcurarAutor () {
	listaAutor *autores = InitListA(autores);

	char autor[100] = {0};
	char resp = 0;
	int posY = 0;
	size_t quant = 0;
	char erro = 0;

	while (1) {
		DrawBegin();

		DrawTextRight("Nome:", TC_GRN);
		DrawTextRight(autor, TC_BLK);
		
		LoadText();
		
		int count = 0;
		DrawTextSpace('r');
		if(!erro){
			for (t_noA *no = autores->inicio; no != autores->fim; no = no->proximo) {
				DrawTextRight(no->autor.nome, count == posY ? TC_RED : TC_BLK);
				count++;
			}
		}

		DrawEnd();
		
		resp = getchar();

		switch (resp) {
			case 10:
				{
					size_t cont = 0;
					for(t_noA *no = autores->inicio; no != autores->fim; no = no->proximo) {
						if (posY == cont) {
							MostrarAutor(no->autor.cod);
							break;
						}
						cont++;
					}
				}
			break;
			case 27:
				if (!kbhit()){
					DeleteListA(autores);
					return;
				}
				getchar();
				resp = getchar();
				switch (resp) {
					case 'A':
						if (posY) posY--;
					break;
					case 'B':
						if (posY + 1 < autores->size) posY++;
					break;
				}
			break;
			case 127:
			case 8:
				if (!quant) break;
				
				autor[--quant] = 0;
				posY = 0;
				
				ClearListA(autores);
				if (quant >= 3) {
					BuscarAutor(autores, autor, 3);
				}
			break;
			default:
				if (resp >= 32 && resp <= 126) {
					if (quant >= 49) break; 
					posY = 0;
					autor[quant++] = resp;
					Upper_Case(autor);
	
					if (quant >= 3) {
						ClearListA(autores);
						BuscarAutor(autores, autor, 10);
					}
				}
			break;
		}
	}
}

void MostrarBiblioteca () {
	char resp = 0;
	int totalPag = 1;
	int pag = 1;
	char posY = 0;
	
	listaLivro* livros = InitList(livros);
	ShowBiblioteca(livros, pag, &totalPag, 24);
	t_noL* no = NULL;
	
	while(1){
		DrawBegin();

		no = livros->inicio;
		for(int i = 0; i<12; i++){
			if(no == livros->fim) DrawTextSpace('l');
			else {
				DrawTextLeft(no->livro.titulo, posY == i ? TC_RED : TC_BLK);
				no = no->proximo;
			}
		}
		for(int i = 0; i<12; i++){
			if(no == livros->fim) DrawTextSpace('r');
			else {
				DrawTextRight(no->livro.titulo, posY == (i + 12) ? TC_RED : TC_BLK);
				no = no->proximo;
			}
		}

		if(pag > 1){
			DrawTextSpace('l');
			DrawTextLeft("←", TC_BLU);
		}
		if(pag < totalPag){
			DrawTextSpace('r');
			DrawTextRight("→", TC_BLU);
		}

		DrawEnd();

		resp = getchar();

		switch(resp){
			case 10:
				{
					size_t cont = 0;
					for(t_noL *no_ = livros->inicio; no_ != livros->fim; no_ = no_->proximo) {
						if (posY == cont) {
							AnimLivro('d');
							MostrarLivro(no_->livro);
							break;
						}
						cont++;
					}
				}
			break;
			case 27:
				if (!kbhit()){
					DeleteList(livros);
					AnimLivro('e');
					return;
				}
				getchar();
				resp = getchar();
				switch (resp) {
					case 'A':
						if (posY) posY--;
					break;
					case 'B':
						if (posY + 1 < livros->size) posY++;
					break;
					case 'C':
						if(pag == totalPag) break;
						pag++;
						posY = 0;
						ClearList(livros);
						ShowBiblioteca(livros, pag, &totalPag, 24);
						AnimLivro('d');
					break;
					case 'D':
						if(pag == 1) break;
						posY = 0;
						pag--;
						ClearList(livros);
						ShowBiblioteca(livros, pag, &totalPag, 24);
						AnimLivro('e');
					break;
				}
			break;
		}
	}
}