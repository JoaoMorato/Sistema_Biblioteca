#ifndef _CRYPTO_
#define _CRYPTO_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ui unsigned int

typedef ui Palavra;
typedef Palavra* Bloco;

//-------------------FUNCIONALIDADES--------------------

Bloco CriaBloco ();
Bloco AdicionaTamanhoBloco (Bloco bloco, ui long tam);
Bloco SenhaBinarioArray (char * senha, int tam);
Bloco Completa80Bits (Bloco bloco);

int X (ui x, ui y, ui z);
int Y (ui x, ui y, ui z);
int Z (ui x, ui y, ui z);

int Referencia (int t, ui x, ui y, ui z);
int SemReferencia (int t);

ui * InicializaH ();
ui * ComputaHashs (Bloco bloco, ui * H);

void PrintaHash (ui * H);
void Criptografar (char senha[41]);

int Compress (int key, int h);
int CriptoRSA (int num, int key);
int DescriptoRSA (int num, int key);

//-----------------------FUNÇÕES------------------------

Bloco CriaBloco () {
	Bloco bloco = (Bloco) malloc (sizeof (Palavra) * 80);
	int i;
	for (i = 0; i < 80; i++) {
		bloco[i] = 0;
	}
	return bloco;
}

Bloco AdicionaTamanhoBloco (Bloco bloco, ui long tam) {
	tam = tam * 8;
	int p1 = (ui) (tam >> 32);
	int p2 = (ui) tam;

	bloco[14] = p1;
	bloco[15] = p2;

	return bloco;
}

Bloco SenhaBinarioArray (char * senha, int tam) {
	int charactere;
	int bloco_pos;
	int maior = 1;
	ui dif;

	Palavra palavra;
	Bloco bloco = (Bloco) CriaBloco ();

	for (charactere = 0; charactere < tam; charactere ++) {
		palavra = 0;
		bloco_pos = charactere / 4;

		do {
			palavra = palavra << 8;
			palavra = palavra + (int) senha [charactere]; 
			charactere ++;

		} while (charactere % 4);

		bloco [bloco_pos] = palavra;
	}
	dif = 4 - tam % 4;
	maior = maior << (dif * 8 -1);

	if (!dif)
		bloco_pos += 1;

	bloco [bloco_pos] += maior;

	AdicionaTamanhoBloco (bloco, tam);

	return bloco;
}

Bloco Completa80Bits (Bloco bloco) {
	int pos = 0;

	do { 
		bloco[16 + pos] = bloco[13 + pos] ^ bloco[8 + pos];
		bloco[16 + pos] = bloco[16 + pos] ^ bloco[2 + pos];
		bloco[16 + pos] = bloco[16 + pos] ^ bloco[pos];
		bloco[16 + pos] = (bloco[16 + pos] >> 31) | ( bloco[16 + pos] << 1); 
		pos ++;

	} while((pos + 16) < 80);

	return bloco;
}

int X (ui x, ui y, ui z) {
	return (x & y) ^ (~x & z);
}

int Y (ui x, ui y, ui z) {
	return (x ^ y) ^ z;
}

int Z (ui x, ui y, ui z) {
	return (x & y) ^ (x & z) ^ (y & z);
}

int Referencia (int t, ui x, ui y, ui z) {
	if (t < 20)
		return X (x, y, z);

	if (t < 40)
		return Y (x, y, z);

	if (t < 60)
		return Z (x, y, z);

	return Y (x, y, z);
}

int SemReferencia (int t) {
	if (t < 20)
		return 0x5a827999;
	if (t < 40)
		return 0x6ed9eba1;
	if (t < 60)
		return 0x8f1bbcdc;
	return 0xca62c1d6;
} 

ui * InicializaH() {
	ui * H = (ui*)malloc(sizeof(ui)*5);

	H [0] = 0x67452301;
	H [1] = 0xefcdab89;
	H [2] = 0x98badcfe;
	H [3] = 0x10325476;
	H [4] = 0xc3d2e1f0;

	return H;
}

ui * ComputaHashs (Bloco bloco, ui * H) {
	ui a = H [0];
	ui b = H [1];
	ui c = H [2];
	ui d = H [3];
	ui e = H [4];

	ui T;
	int t;

	for (t = 0; t < 80; t++) {
		T  = (a >> 27) | (a << 5);
		T  = T + Referencia (t, b, c, d);
		T  = T + e;
		T  = T + SemReferencia (t);
		T  = T + bloco [t];

		e  = d;
		d  = c;
		c  = (b >> 2) | (b << 30);
		b  = a;
		a  = T;

		H[0] += a;
		H[1] += b;
		H[2] += c;
		H[3] += d;
		H[4] += e;
	}

	return H;
}

void PrintaHash (ui * H) {
	int i;
	FILE *ftemp = fopen ("temp.txt", "w+");

	for(i = 0; i < 5; i++)
		fprintf (ftemp, "%x", H[i]);
	fclose (ftemp);
}

void Criptografar (char senha[41]) {
	Bloco bloco = SenhaBinarioArray (senha, strlen(senha));
	bloco = Completa80Bits (bloco);

	unsigned int * H = InicializaH (); 
	H = ComputaHashs (bloco, H);
	PrintaHash (H);

	FILE *ftemp = fopen ("temp.txt", "r+");

	memset (senha, 0, 41);
	fgets (senha, 41, ftemp);

	fclose (ftemp);
	remove ("temp.txt");
}

int Comprime (int chave, int h) {
	int cmp;

	while(1) {
		cmp= chave % h;

		if(cmp == 0)
			return h;
		chave = h;
		h = cmp;
	}
}

int CriptoRSA (int num, int chave) {
	int p = 3;
	int q = 7;
	int k =2;
	int n = (p * q);
	int h = (p - 1) * (q - 1);
	int count;
	int d;
	double cripto;

	while(chave < h) {
		count = Comprime(chave, h);

		if (count == 1)
			break;
		else
			chave++;
	}
	d = (1 + (k * h)) / chave;

	cripto = pow (num, chave);
	cripto = fmod (cripto, n);

	return cripto;
}

int DescriptoRSA (int num, int chave) {
	int p = 3;
	int q = 7;
	int n = (p * q);
	int h = (p - 1) * (q - 1);
	int k =2;
	int count;
	int d;
	double desCripto;

	while (chave < h) {
		count = Comprime(chave, h);

		if (count == 1)
			break;
		else
			chave++;
	}
	d = (1 + (k * h)) / chave;

	desCripto = pow (num, d);
	desCripto = fmod (desCripto, n);

	return desCripto;
}

#endif