#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <ctype.h>
#include "Huffman.h"

#define YELLOW 14
#define BLUE 1
#define BLACK 0

/*--------------------- Tree ---------------------*/
void construirSubTree(Tree* *Raiz, int simbolo, char *codHuff, int *i){
	if(codHuff[*i] != '\0')
	{
		if(*Raiz == NULL)
			*Raiz = criaNo(-1, 0);
		
		if(codHuff[*i] == '0')
		{
			(*i)++;	// Incrementa o índice antes de ir para a esquerda
			construirSubTree(&(*Raiz)->esq, simbolo, codHuff, i);
		}
		else
		{
			(*i)++;	// Incrementa o índice antes de ir para a direita
			construirSubTree(&(*Raiz)->dir, simbolo, codHuff, i);
		}
	}
	else
		*Raiz = criaNo(simbolo, 0);
}

Tree *construirTree(FILE *Ptr){
	TabReg R;
	Tree *Raiz = criaNo(-1,0);
	int i;
	
	fread(&R, sizeof(TabReg), 1, Ptr);
	while(!feof(Ptr))					//Varredura no Arq Binario
	{
		i = 1;
		if(R.codHuff[0] == '0')
			construirSubTree(&Raiz->esq, R.simbolo, R.codHuff, &i);
		else
			construirSubTree(&Raiz->dir, R.simbolo, R.codHuff, &i);
		
		fread(&R, sizeof(TabReg), 1, Ptr);
	}
	return Raiz;
}

void buscarSimbolo(int simbolo, char *palavra) {
    FILE *Ptr = fopen("TabReg.dat", "rb");
    TabReg Reg;

    fseek(Ptr, simbolo*sizeof(TabReg), 0);
    fread(&Reg, sizeof(TabReg), 1, Ptr);

    strcat(palavra, Reg.palavra);
    fclose(Ptr);
}

void construirFrase(Tree *Raiz, char *frase, char *codificada) {
    FILE *Ptr = fopen("Frase.dat", "rb");
    Tree *aux = Raiz;
    TpByte B;
    char byte[9];
    unsigned int i;

    fread(&B.info, sizeof(char), 1, Ptr);
    while (!feof(Ptr)) 
	{
        sprintf(byte, "%d%d%d%d%d%d%d%d", B.bi.b0, B.bi.b1, B.bi.b2, B.bi.b3, B.bi.b4, B.bi.b5, B.bi.b6, B.bi.b7);
        strcat(codificada, byte);
        strcat(codificada, " ");
        for (i = 0; i < strlen(byte); i++) 
		{
            if (aux != NULL) 
			{
                if (byte[i] == '0')
                    aux = aux->esq;
                else
                    aux = aux->dir;

                if (aux->simbolo != -1) 
				{
                    buscarSimbolo(aux->simbolo, frase);
                    aux = Raiz;
                }
            }
            else
                aux = Raiz;
        }
        fread(&B.info, sizeof(char), 1, Ptr);
    }
    fclose(Ptr);
}
/*------------------------------------------------*/
void configurarConsole(int largura, int altura) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT janela = {0, 0, largura - 1, altura - 1};
    COORD tamanhoBuffer = {largura, altura};

    // Ajusta o tamanho do buffer
    SetConsoleScreenBufferSize(hOut, tamanhoBuffer);

    // Ajusta o tamanho da janela
    SetConsoleWindowInfo(hOut, TRUE, &janela);

    // Maximiza o console
    HWND console = GetConsoleWindow();
    ShowWindow(console, SW_MAXIMIZE);
}

void Moldura(int CI, int LI, int CF, int LF, int CorT, int CorF) 
{
    int i;
    textcolor(CorT);
    textbackground(CorF);
    gotoxy(CI, LI); printf("%c", 201);
    gotoxy(CI, LF); printf("%c", 200);
    gotoxy(CF, LI); printf("%c", 187);
    gotoxy(CF, LF); printf("%c", 188);

    for (i = CI + 1; i < CF; i++) {
        gotoxy(i, LI); printf("%c", 205);
        gotoxy(i, LF); printf("%c", 205);
    }
    for (i = LI + 1; i < LF; i++) {
        gotoxy(CI, i); printf("%c", 186);
        gotoxy(CF, i); printf("%c", 186);
    }
    textcolor(7);
    textbackground(0);
}

void preencherTelaAzul() {
    int linhas = 30; // Máximo de linhas para o console
    int colunas = 120; // Máximo de colunas para o console
    int i, j;

    textbackground(BLUE); 
    clrscr(); // Limpa a tela com a cor de fundo configurada

    for (i = 1; i <= linhas; i++) {
        for (j = 1; j <= colunas; j++) {
            gotoxy(j, i); 
            printf(" "); 
        }
    }
    gotoxy(120,30);
}

int main()
{
	configurarConsole(120,30);
    
    FILE *Ptr = fopen("TabReg.dat", "rb");
    Tree *Raiz;
    int i=5;
    char fraseDecodificar[1000]="";
    char fraseCodificada[1000]="";

    Raiz = construirTree(Ptr);
    construirFrase(Raiz, fraseDecodificar, fraseCodificada);

    rewind(Ptr);
    gotoxy(1,1);
 
	/*------------------------------------------ PRIMEIRA TELA ----------------------------------------------------------*/
	system("cls");
	preencherTelaAzul();
	Moldura(3,2,118,4,BLACK,BLUE); //Borda Titulo
	textbackground(BLUE);
	textcolor(YELLOW);
	gotoxy(12,3); printf("#### ARVORE HUFFMAN - DESENVOLVIDO POR LEANDRO M. C. RODRIGUES - PROFESSOR FRANCISCO DE ASSIS ####");
	Moldura(52,4,67,6,BLACK,BLUE); //Borda SubTitulo
	textbackground(BLUE);
	textcolor(YELLOW);
	gotoxy(53,5); printf("DESC0MPACTAD0R");
	textbackground(BLUE);
	textcolor(YELLOW);
    i++; gotoxy(2, i);
    printf("Arvore (Horizontal):\n\n");
    exibirTreeHorizontal(Raiz);
    getch();
    /*-------------------------------------------------------------------------------------------------------------------*/
    
    /*------------------------------------------ SEGUNDA TELA ----------------------------------------------------------*/
    system("cls");
	preencherTelaAzul();
	Moldura(3,2,118,4,BLACK,BLUE); //Borda Titulo
	textbackground(BLUE);
	textcolor(YELLOW);
	gotoxy(12,3); printf("#### ARVORE HUFFMAN - DESENVOLVIDO POR LEANDRO M. C. RODRIGUES - PROFESSOR FRANCISCO DE ASSIS ####");
	Moldura(52,4,67,6,BLACK,BLUE); //Borda SubTitulo
	textbackground(BLUE);
	textcolor(YELLOW);
	gotoxy(53,5); printf("DESC0MPACTAD0R");
	textbackground(BLUE);
	textcolor(YELLOW);
    exibirRegistroBinario(Ptr);
	printf("\n\nFrase Huffman: %s", fraseCodificada);
    printf("\n\nFrase Decodificada: %s\n\n", fraseDecodificar);
    getch();
    /*-------------------------------------------------------------------------------------------------------------------*/
    
    fclose(Ptr);
    textbackground(BLACK);
	system("cls");
	gotoxy(50,15);
	return 0;
    return 0;
    
}