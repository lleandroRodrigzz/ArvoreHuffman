#include <conio.h>
#include <conio2.h>
#define TFC 50

struct bits {
	unsigned char b7:1;		//menos significativo
	unsigned char b6:1;
	unsigned char b5:1;
	unsigned char b4:1;
	unsigned char b3:1;
	unsigned char b2:1;
	unsigned char b1:1;
	unsigned char b0:1;		//mais significativo
};

union byte {
	struct bits bi;
	unsigned char info;
};
typedef union byte Byte;

struct tree {
	int simbolo, frequencia;
	struct tree *esq, *dir;
};
typedef struct tree Tree;

struct floresta {
	Tree *no;
	struct floresta *prox;
};
typedef struct floresta Floresta;

struct registro {
	char palavra[TFC];
	int frequencia;
	int simbolo
	char codHuff[TFC];
	struct registro *prox;
};
typedef struct registro TabReg;

/*--------------------- Byte --------------------*/
void exibirByte(TpByte *B) {
	printf(" %d", B->bi.b0);
	printf("%d", B->bi.b1);
	printf("%d", B->bi.b2);
	printf("%d", B->bi.b3);
	printf("%d", B->bi.b4);
	printf("%d", B->bi.b5);
	printf("%d", B->bi.b6);
	printf("%d", B->bi.b7);
}
/*-----------------------------------------------*/

/*------------------- Tree ----------------------*/
Tree *criaNo(int simbolo, int frequencia) {
	Tree *no = (Tree*)malloc(sizeof(Tree));
	no->simbolo = simbolo;
	no->frequencia = frequencia;
	no->esq = no->dir = NULL;
	return no;
}

void exibirTreeHorizontal(Tree *raiz) {
	static int n = -1, i;
	if(raiz != NULL) {
		n++;
		exibirTreeHorizontal(raiz->dir);
		for(i=0; i < 5*n; i++)
			printf(" ");
		if(raiz->simbolo != -1)
			printf("(%d,%d)\n", raiz->simbolo, raiz->frequencia);
		else
			printf("(#,%d)\n", raiz->frequencia);
		exibirTreeHorizontal(raiz->esq);
		n--;
	}
}

void exibirTreeVertical(Tree *raiz, int x, int y, int deslocamento) {
	int novoXEsq, novoXDir, novoY;
	if (raiz != NULL) 
	{
		// Posiciona o cursor e imprime o nó atual
		gotoxy(x, y);
		// Verifica se o nó é folha ou interno
		if (raiz->simbolo != -1) {
			// Nó folha: exibe símbolo e frequência
			printf("(%d,%d)", raiz->simbolo, raiz->frequencia);
		} else {
			// Nó interno: exibe uma indicação padrão e a frequência
			printf("(*,%d)", raiz->frequencia);
		}
		// Calcula a posição dos filhos
		novoXEsq = x - deslocamento;
		novoXDir = x + deslocamento;
		novoY = y + 2;
		// Exibe as subárvores esquerda e direita com recuo
		exibirTreeVertical(raiz->esq, novoXEsq, novoY, deslocamento / 2);
		exibirTreeVertical(raiz->dir, novoXDir, novoY, deslocamento / 2);
	}
}

void limparTree(Tree **Raiz) {
	Tree *aux;
	if(*Raiz != NULL) {
		aux = *Raiz;
		limparTree(&(*Raiz)->esq);
		limparTree(&(*Raiz)->dir);
		free(aux);
	}
}
/*-----------------------------------------------*/

/*------------------- Floresta ----------------------*/
Floresta *criarNoFlo(Tree *no) {
	Floresta *Flo = (Floresta*)malloc(sizeof(Floresta));
	Flo->no = no;
	return Flo;
}

void inserirFloresta(Floresta **Flo, Tree *no) {
	Floresta *aux, *atual, *ant;

	ant = NULL;
	aux = criarNoFlo(no);
	atual = *Flo;
	while (atual != NULL && aux->no->frequencia >= atual->no->frequencia) {
		ant = atual;
		atual = atual->prox;
	}

	if (ant == NULL) {
		aux->prox = *Flo;
		*Flo = aux;
	} else {
		ant->prox = aux;
		aux->prox = atual;
	}
}

Tree *excluirMenorFloresta(Floresta **Flo) {
	Tree *no;
	Floresta *aux, *ant, *auxMenor, *auxAnt;
	int menorFreq=9999;

	auxMenor = NULL;
	auxAnt = NULL;
	ant = NULL;
	aux = *Flo;
	while (aux != NULL) {
		if(aux->no->frequencia < menorFreq) {
			menorFreq = aux->no->frequencia;
			auxAnt = ant;
			auxMenor = aux;
		}
		ant = aux;
		aux = aux->prox;
	}

	no = auxMenor->no;  // Guarda o nó com menor frequência
	if (auxAnt == NULL)
		*Flo = (*Flo)->prox; // Atualiza a cabeça da floresta
	else
		auxAnt->prox = auxMenor->prox; // Conecta o nó anterior ao próximo

	free(auxMenor);  // Libera o nó
	return no;
}

void exibirFloresta(Floresta *Flo) {
	int i=3;
	gotoxy(60, 1);
	printf("//### Floresta ###//");
	while (Flo != NULL) {
		gotoxy(67, i);
		printf("%d", Flo->no->simbolo);
		gotoxy(72, i);
		printf("%d", Flo->no->frequencia);
		Flo = Flo->prox;
		i++;
	}
}
/*---------------------------------------------------*/

/*--------------------- Registro -------------------------*/
TabReg *criarNoReg(char *palavra) {
	TabReg *Reg = (TabReg*)malloc(sizeof(TabReg));
	strcpy(Reg->palavra, palavra);
	Reg->frequencia = 1;
	Reg->prox = NULL;
	return Reg;
}

void inserirRegistro(TabReg* *Reg, char *palavra) {
	TabReg *aux, *ant;

	ant = NULL;
	aux = *Reg;
	if (aux == NULL)
		*Reg = criarNoReg(palavra);
	else {
		while (aux != NULL && stricmp(aux->palavra, palavra) != 0) {
			ant = aux;
			aux = aux->prox;
		}
		if (aux == NULL)
			ant->prox = criarNoReg(palavra);
		else
			aux->frequencia++;
	}
}

void ordenarRegistro(TabReg* *Reg) {
	TabReg *aux, *temp, *ant;
	int trocou = 1;

	while(trocou) {
		trocou = 0;
		ant = NULL;
		aux = *Reg;
		while (aux->prox != NULL) {
			if (aux->frequencia > aux->prox->frequencia) {
				if (ant == NULL) // Se o nó atual for o primeiro, ajusta o ponteiro do início
					*Reg = aux->prox;
				else // Caso contrário, o ponteiro do nó anterior aponta para o próximo
					ant->prox = aux->prox;

				temp = aux->prox;
				aux->prox = temp->prox;
				temp->prox = aux;
				aux = temp;
				trocou = 1;
			}
			ant = aux;
			aux = aux->prox;
		}
	}
}

void exibirRegistro(TabRegistro *Reg, int *i){
	gotoxy(1,1);
	printf("//##################### Registro #######################//");
	gotoxy(3, 3); printf("|Simbolo|");
	gotoxy(14, 3); printf("|Palavra|");
	gotoxy(26, 3); printf("|Frequencia|");
	gotoxy(41, 3); printf("|Codigo-Huffman|");
    while(Reg != NULL){
		gotoxy(6, *i);   printf("%d", Reg->simbolo);
		gotoxy(15, *i);  
		if(Reg->palavra[0] != 32)
			printf("%s", Reg->palavra);
		else
			printf("ESPACO");
		gotoxy(30, *i);  
		printf(" %d", Reg->frequencia);
		gotoxy(43, *i);  printf("%s", Reg->codigoHuff);
        Reg = Reg->prox;
		(*i)+=2;
    }
}

void exibirRegistroBinario(FILE *ptr, int *i){
	TabReg Reg;
	gotoxy(1,1);
	printf("//--------------------- Registro -----------------------//");
			  
	gotoxy(3, 3); printf("|Simbolo|");
	gotoxy(14, 3); printf("|Palavra|");
	gotoxy(26, 3); printf("|Frequencia|");
	gotoxy(41, 3); printf("|Codigo-Huffman|");
	fread(&Reg, sizeof(TabReg), 1, ptr);
    while(!feof(ptr)){
		gotoxy(6, *i);   printf("%d", Reg.simbolo);
		gotoxy(15, *i);  
		if(Reg.palavra[0] != 32)
			printf("%s", Reg.palavra);
		else
			printf("ESPACO");
		gotoxy(30, *i);  
		printf(" %d", Reg.frequencia);
		gotoxy(43, *i);  printf("%s", Reg.codigoHuff);
		(*i)+=2;
		fread(&Reg, sizeof(TabReg), 1, ptr);
    }
}

void limparRegistro(TabReg* *Reg){
    if (*Reg != NULL) {
        TabReg *aux = *Reg;
        *Reg = (*Reg)->prox;
        free(aux);
        limparRegistro(Reg);
    }
}
/*--------------------------------------------------------*/