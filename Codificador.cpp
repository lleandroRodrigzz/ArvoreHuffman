#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <ctype.h>
#include "Huffman.h"

/*--------------------- Frase --------------------*/
void lerFrase(TabReg* *Reg, char *frase){
	unsigned int i;
	int tamFrase = strlen(frase);
	int pos;
	char palavra[TFC];
	for(i = 0; i < tamFrase; i++)
	{
		for(pos = 0; i < tamFrase && frase[i] != 32; pos++, i++)
		{
			palavra[pos] = frase[i];
		}
		palavra[pos] = '\0';
		inserirRegistro(&*Reg, palavra);
		if(frase[i] == 32)
			inserirRegistro(&*Reg," ");
	}
}
/*------------------------------------------------*/

/*-------------------- Floresta ------------------*/
void criarFloresta(Floresta* *Flore, TabReg *Reg){
	int i;
	Tree *no;
	
	for(i = 0; Reg != NULL; i++){
		no = criaNo(i, Reg->frequencia);
		inserirFloresta(&(*Flore), no);
		Reg->simbolo = i;
		Reg = Reg -> prox;
	}
}
/*------------------------------------------------*/

/*-------------------- Tree ----------------------*/
void criaTree(Tree* *Raiz, Floresta* *Flore){
	Tree *aux, *aux1, *novo;
	
	while((*Flore)->prox != NULL){
		aux = excluirMenorFloresta(&*Flore);
		aux1 = excluirMenorFloresta(&*Flore);
		novo = criaNo(-1, aux->frequencia + aux1->frequencia);
		novo->esq = aux;
		novo->dir = aux1;
		inserirFloresta(&*Flore, novo);
	}
	*Raiz = (*Flore)->no;
}
/*------------------------------------------------*/

/*------------------ Huffman Code ----------------*/
void setCodigo(TabReg *Reg, int simbolo, char *cod){
	while(Reg->simbolo != simbolo){
		Reg = Reg->prox;
	}
	strcpy(Reg->codHuff, cod);
}

void criarCodigo(TabReg* *Reg, Tree *Raiz, char *cod){
	char codEsq[TFC], codDir[TFC];
	
	if(Raiz != NULL){
		if(Raiz->esq == NULL && Raiz->dir == NULL)
			setCodigo(*Reg, Raiz->simbolo, cod);
		else{
			strcpy(codEsq, cod);
			criarCodigo(&(*Reg), Raiz->esq, strcat(codEsq, "0"));
			strcpy(codDir, cod);
			criarCodigo(&(*Reg), Raiz->dir, strcat(codDir, "1"));
		}
	}
}
/*------------------------------------------------*/

/*--------------------- Binario ------------------*/
void criarArqRegBin(TabReg *Reg){
	FILE *Ptr = fopen("TabReg.dat","wb");
	while(Reg != NULL){
		fwrite(Reg, 1, sizeof(TabReg), Ptr);
		Reg = Reg->prox;
	}
	fclose(Ptr);
}

void completaBits(TabReg *Reg, TpByte *B, int *cont, FILE *Ptr){
	int i;
	
	char *codHuff = Reg->codHuff;
	
	for(i = 0; codHuff[i] != '\0'; i++){
		B->info <<= 1;
		if(codHuff[i] == '1')
			B->bi.b7 = 1;
		else
			B->bi.b0 = 0;
		(*cont)++;
		if(*cont == 8){
			exibirByte(B);
			fwrite(&B->info, sizeof(char), 1, Ptr);
			*cont = 0;
			B->info = 0;
		}
	}
}

void criaArqFraseBin(TabReg *Reg, char *frase){
	FILE *Ptr = fopen("Frase.dat","ab+");
	int i, pos, cont = 0;
	char palavra[TFC];
	TpByte B;
	TabReg *auxSpace = Reg, *aux;
	
	while(auxSpace != NULL && strcmp(auxSpace->palavra, " ") != 0)
		auxSpace = auxSpace->prox;
		
	B.info = 0;
	for(i = 0; frase[i] != '\0'; i++){
		for(pos = 0; frase[i] != '\0' && frase[i] != 32; pos++, i++)
			palavra[pos] = frase[i];
		palavra[pos] = '\0';
		aux = Reg;
		while(aux != NULL && stricmp(palavra, aux->palavra) != 0)
			aux = aux->prox;
		
		if(aux != NULL){
			completaBits(aux, &B, &cont, Ptr);
			if(frase[i] == 32)
				completaBits(auxSpace, &B, &cont, Ptr);
		}
	}
	
	while(cont < 8){
		B.info <<= 1;
		B.bi.b7 = 0;
		cont++;
	}
	
	exibirByte(&B);
	fwrite(&B.info, sizeof(char), 1, Ptr);
	fclose(Ptr);
}
/*------------------------------------------------*/

/*------------------- Interface//Main ------------------*/
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

void LimpaMoldura(int CI, int LI, int CF, int LF, int CorF) {
    int i, j;

    textbackground(CorF); 
    for (j = LI + 1; j < LF; j++) 
	{ 
        for (i = CI + 1; i < CF; i++) 
		{ 
            gotoxy(i, j); 
            printf(" "); 
        }
    }
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
	FILE *Ptr = NULL;
	Tree *Raiz = NULL;
	Floresta *Flore = NULL;
	TabReg *Reg = NULL;
	char *frase;
	
	configurarConsole(120,30);
	
	remove("Frase.dat");
	remove("TabReg.dat");
	
	frase = "viver e aprender crescer a cada dia tornando cada momento unico especial e uma chance de ser feliz";
	
	preencherTelaAzul();
	textcolor(YELLOW);
	
	/*------------------------------------------ PRIMEIRA TELA ----------------------------------------------------------*/
	Moldura(3,2,118,4,BLACK,BLUE); //Borda Titulo
	textbackground(BLUE);
	textcolor(YELLOW);
	gotoxy(12,3); printf("#### ARVORE HUFFMAN - DESENVOLVIDO POR LEANDRO M. C. RODRIGUES - PROFESSOR FRANCISCO DE ASSIS ####");
	
	Moldura(54,4,66,6,BLACK,BLUE); //Borda SubTitulo
	textbackground(BLUE);
	textcolor(YELLOW);
	gotoxy(55,5); printf("COMPACTADOR");
	
	lerFrase(&Reg, frase);
	ordenarRegistro(&Reg);
	
	textbackground(BLUE);
	textcolor(YELLOW);
	criarFloresta(&Flore, Reg);
	exibirFloresta(Flore);
	
	criaTree(&Raiz, &Flore);
	criarCodigo(&Reg, Raiz, "");
	
	criarArqRegBin(Reg);
    Ptr = fopen("TabReg.dat", "rb");
	exibirRegistroBinario(Ptr);	
	fclose(Ptr);
	getch();
	/*-------------------------------------------------------------------------------------------------------------------*/
	
	/*------------------------------------------ SEGUNDA TELA ----------------------------------------------------------*/
	system("cls");
	preencherTelaAzul();
	Moldura(3,2,118,4,BLACK,BLUE); //Borda Titulo
	textbackground(BLUE);
	textcolor(YELLOW);
	gotoxy(12,3); printf("#### ARVORE HUFFMAN - DESENVOLVIDO POR LEANDRO M. C. RODRIGUES - PROFESSOR FRANCISCO DE ASSIS ####");
	Moldura(54,4,66,6,BLACK,BLUE); //Borda SubTitulo
	textbackground(BLUE);
	textcolor(YELLOW);
	gotoxy(55,5); printf("COMPACTADOR");
	textbackground(BLUE);
	textcolor(YELLOW);
	gotoxy(3,7);
	printf("Arvore (Horizontal):\n\n");
	exibirTreeHorizontal(Raiz);
	getch();
	/*-------------------------------------------------------------------------------------------------------------------*/
	
	/*------------------------------------------ TERCEIRA TELA ----------------------------------------------------------*/
	system("cls");
	Moldura(3,2,118,4,BLACK,BLUE); //Borda Titulo
	textbackground(BLUE);
	textcolor(YELLOW);
	gotoxy(12,3); printf("#### ARVORE HUFFMAN - DESENVOLVIDO POR LEANDRO M. C. RODRIGUES - PROFESSOR FRANCISCO DE ASSIS ####");
	Moldura(54,4,66,6,BLACK,BLUE); //Borda SubTitulo
	textbackground(BLUE);
	textcolor(YELLOW);
	gotoxy(55,5); printf("COMPACTADOR");
	textbackground(BLUE);
	textcolor(YELLOW);
	gotoxy(4,8);printf("Frase: %s", frase);
	gotoxy(4,10);printf("Huffman:"); criaArqFraseBin(Reg, frase);
	
	
	gotoxy(4,15);printf("Tabela de Registros foi Gravada em 'TabReg.dat' com sucesso...");
	gotoxy(4,17);printf("Frase Codificada foi Gravada em 'Frase.dat' com sucesso...");
	/*-------------------------------------------------------------------------------------------------------------------*/
	
	limparTree(&Raiz);
	limparRegistro(&Reg);
	free(Flore->no);
	free(Flore);
	
	getch();
	textbackground(BLACK);
	system("cls");
	gotoxy(50,15);
	return 0;
}
/*------------------------------------------------*/