#define _CRT_SECURE_NO_WARNINGS

#define inf INT_MAX

#define LEFT -1
#define RIGHT 1
#define UP 2
#define DOWN -2

#define ZID '#'
#define PUT '-'
#define PECURKA 'P'
#define IGRAC 'S'
#define CILJ '*'

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>


typedef struct Cvor {
	int distance;
	int broj_cvora;
	struct Cvor* sledeci;
}Cvor;

typedef struct Tacka {
	int row;
	int column;
}Tacka;//strukutura za skladistenje pocetne tacke i krajnje tacke


char mapa[6][6];
Tacka pocetak;
Tacka kraj;
Tacka pecurka;
int udarenaPecurka = 0;


FILE* ptr = NULL;
int indeksPocetka;
int playGame = 0;
int run = 1; //condition for game while loop
int win = 0;
int playerDistance = 0;
int computerDistance = 0;
int matrica[6][6];
int graf[36][36] = { 0 };
int visited[36] = { 0 };
int distance[36] = { 0 };
int previous[36] = { 0 };
Cvor* pq = NULL;//prioritetni red

Cvor* napraviNovi(int distance, int broj_cvora) {
	Cvor* novi = (Cvor*)malloc(sizeof(Cvor));
	if (novi == NULL) return NULL;

	novi->sledeci = NULL;
	novi->distance = distance;
	novi->broj_cvora = broj_cvora;

	return novi;
}
void insertAfter(Cvor* p, Cvor* q) {
	Cvor* pom = p->sledeci;
	p->sledeci = q;
	q->sledeci = pom;
}
void ubaciURed(Cvor** lista, Cvor* novi) {
	Cvor* pomocni = *lista;
	Cvor* pom2 = NULL;

	while (pomocni != NULL && pomocni->distance <= novi->distance) {
		pom2 = pomocni;
		pomocni = pomocni->sledeci;
	}
	//insertAfter
	if (pom2 == NULL) {
		Cvor* pom3 = *lista;
		(*lista) = novi;
		novi->sledeci = pom3;
		return;
	}
	insertAfter(pom2, novi);
}
Cvor* deque(Cvor** lista) {
	if (*lista == NULL) {
		return NULL;
	}
	else if ((*lista)->sledeci == NULL) {
		Cvor* pom = (*lista);
		(*lista) = NULL;
		return pom;
	}
	Cvor* pom = (*lista);
	(*lista) = (*lista)->sledeci;

	return pom;
}
void ispis(Cvor* lista) {
	Cvor* pom = lista;

	while (pom != NULL) {
		printf("%d Cvor: %d", pom->distance, pom->broj_cvora);
		pom = pom->sledeci;
	}
	printf("\n");
}


int ucitajMapuIzFajla(int argc, char* argv[]) {
	if (argc == 1) {
		printf("Niste uneli nijedan parametar, pokusajte ponovo\n");
		return 0;
	}
	else if (argc == 2) {
		ptr = fopen(argv[1], "r");
	}
	else if (argc == 3 && strcmp(argv[2], "-play") == 0) {
		playGame = 1;
		ptr = fopen(argv[1], "r");
	}

	char ch;
	int row = 0;
	int column = 0;


	do {
		ch = fgetc(ptr);
		if (ch == '\n') {
			printf("\n");
			row++;
			column = 0;
			continue;
		}
		else if (ch == EOF) break;

		mapa[row][column] = ch;

		if (ch == '#') {
			matrica[row][column] = 0;
		}
		else {
			matrica[row][column] = 1;
		}
		if (ch == 'S') {
			pocetak.row = row;
			pocetak.column = column;
		}
		else if (ch == 'P') {
			pecurka.row = row;
			pecurka.column = column;
		}
		else if(ch == '*') {
			kraj.row = row;
			kraj.column = column;
		}



		printf("%c", ch);
		column++;

	} while (ch != EOF);
	printf("\n");
	return 1;
}

void generisiMatricuGrafaNaOsnovuMape() {
	for (int i = 1; i < 5; i++) {
		for (int j = 1; j < 5; j++) {
			int trenutni = i * 6 + j;
			if (matrica[trenutni / 6][trenutni % 6] == 0) continue;
			int indeksGore = (i - 1) * 6 + j;
			int indeksDole = (i + 1) * 6 + j;
			int indeksLevo = i * 6 + (j - 1);
			int indeksDesno = i * 6 + (j + 1);
			graf[trenutni][indeksGore] = matrica[indeksGore / 6][indeksGore % 6];
			graf[trenutni][indeksDole] = matrica[indeksDole / 6][indeksDole % 6];
			graf[trenutni][indeksLevo] = matrica[indeksLevo / 6][indeksLevo % 6];
			graf[trenutni][indeksDesno] = matrica[indeksDesno / 6][indeksDesno % 6];

		}
	}
}
void inicijalizacijaDijkstrinogAlgoritma() {
	for (int i = 0; i < 36; i++) {
		distance[i] = inf;
		previous[i] = inf;
	}
	indeksPocetka = pocetak.row * 6 + pocetak.column;
	visited[indeksPocetka] = 1;
	distance[indeksPocetka] = 0;
	previous[indeksPocetka] = -1;

	for (int i = 0; i < 36; i++) {
		if (!visited[i] && graf[indeksPocetka][i] == 1) {
			if (distance[indeksPocetka] + 1 < distance[i]) {
				distance[i] = distance[indeksPocetka] + 1;
				previous[i] = indeksPocetka;
				ubaciURed(&pq, napraviNovi(distance[i], i));
			}
		}
	}
}
void DijkstrinAlgoritam() {
	while (pq != NULL) {
		Cvor* trenutni = deque(&pq);
		visited[trenutni->broj_cvora] = 1;
		//poseti sve susede trenutnog cvora
		for (int i = 0; i < 36; i++) {
			if (!visited[i] && graf[trenutni->broj_cvora][i] == 1) {
				//update distance
				if (distance[trenutni->broj_cvora] + 1 < distance[i]) {
					distance[i] = distance[trenutni->broj_cvora] + 1;
					previous[i] = trenutni->broj_cvora;
					ubaciURed(&pq, napraviNovi(distance[i], i));
				}
			}
		}
	}
	//update computerDistance
	computerDistance = distance[kraj.row * 6 + kraj.column];
}

void printMapu(char mapa[6][6]) {
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n");
	printf("\t\t\n");
	for (int i = 0; i < 6; i++) {
		printf("\t\t\t\t\t\t\t");
		for (int j = 0; j < 6; j++) {
			printf("%c", mapa[i][j]);
		}
		printf("\n");
	}
}
void printMapuBezNovogReda(char mapa[6][6]) {
	printf("\n");
	for (int i = 0; i < 6; i++) {
		printf("\t\t\t\t\t\t\t");
		for (int j = 0; j < 6; j++) {
			printf("%c", mapa[i][j]);
		}
		printf("\n");
	}
}
int proveraDaLiPostojiPutanjaOdTackeAdoTackeB(int indeksKraja) {
	if (previous[indeksKraja] == -1 || previous[indeksKraja] == inf) {
		printf("ne postoji putanja od S do *\n");
		return 0;
	}
	return 1;
}
void putanjaOdTackeAdoTackeB(int* previous, int indekPocetka, int indeksKraja, char mapa[6][6]) {


	int i = indeksKraja;
	while (previous[i] != -1) {
		int row = previous[i] / 6;
		int column = previous[i] % 6;
		mapa[row][column] = '*';
		i = previous[i];
	}
	printf("\t\t\t----------------------------Resenje mape---------------------------\n");
	printMapuBezNovogReda(mapa);
	printf("\t\t\t-------------------------------------------------------------------\n");
}

void delay() {
	for (int i = 0; i < 10000000; i++) {

	}
}
void intro() {
	system("cls");
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n");
	printf("\t\t\t\t\t\t\t LAVIRINT-MILAN NIKOLIC\n");
	printf("\t\t");
	for (int i = 0; i < 100; i++) {
		delay();
		printf("\033[1;32m|\033[1;0m");
	}
	system("cls");

}
void showGameControls() {
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n");
	printf("\t\t\t------------------------------LAVIRINT-------------------------------\n");
	printf("\t\t\t1. Kreci se pomocu slova(\033[1;32m'w'\033[1;0m - gore, \033[1;32m's'\033[1;0m - dole, \033[1;32m'a'\033[1;0m - levo, \033[1;32m'd'\033[1;0m - desno)\n");
	printf("\t\t\t1. Pritisni taster \033[1;31m'q'\033[1;0m ako zelis da prekines igru\n");
	printf("\t\t\t2. Ti se nalazis na mestu S\n");
	printf("\t\t\t3. Cilj se nalazi na mestu *\n");
	printf("\t\t\t4. Ne mozes da se kreces dijagonalno\n");
	printf("\t\t\t----------------------------------------------------------------------\n");
	printf("\t\t\t\033[1;32mPRITISNI BILO KOJI TASTER DA NASTAVIS\033[1;0m");
	getch();
	system("cls");
	
}
void showEndResults() {
	system("cls");
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n");
	printf("\t\t\t\t\tCOMPUTER DISTANCE:%d\n", computerDistance);
	printf("\t\t\t\t\tYOUR DISTANCE:%d\n", playerDistance);

	if (playerDistance <= computerDistance) {
		printf("\t\t\t\t\t\033[1;32mPUTANJA KOJOM STE SE KRETALI JE NAJKRACA\033[1;0m\n");
	}
	else {
		printf("\t\t\t\t\t\033[1;31mPUTANJA KOJOM STE SE KRETALI NIJE NAJKRACA\033[1;0m\n");
	}
	
	printf("\t\t\t\t\t\033[1;32mCOMPUTER SOLUTION:\n");
	putanjaOdTackeAdoTackeB(previous, indeksPocetka, kraj.row * 6 + kraj.column, mapa);
	printf("\033[1;0m\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

void move(int direction) {
	switch (direction)
	{
		case LEFT:

			if (mapa[pocetak.row][pocetak.column - 1] != ZID) {
				if (mapa[pocetak.row][pocetak.column - 1] == CILJ) {
					run = 0;
					win = 1;
					playerDistance++;
				}
				else if (pocetak.row == pecurka.row && (pocetak.column - 1) == pecurka.column) {
					delay();
					//pronadji mesto gde mozes da ides
					if ((mapa[pocetak.row][pocetak.column + 1]) == PUT) {
						mapa[pocetak.row][pocetak.column] = PUT;
						pocetak.column++;
						printf("OVde je usao");
						mapa[pocetak.row][pocetak.column] = IGRAC;
						playerDistance++;
					}
					else if ((mapa[pocetak.row - 1][pocetak.column]) == 1) {
						mapa[pocetak.row][pocetak.column] = PUT;
						pocetak.row--;
						mapa[pocetak.row][pocetak.column] = IGRAC;
						playerDistance++;
					}
					else if ((mapa[pocetak.row + 1][pocetak.column]) == 1) {
						mapa[pocetak.row][pocetak.column] = PUT;
						pocetak.row++;
						mapa[pocetak.row][pocetak.column] = IGRAC;
						playerDistance++;
					}

					//update mapu bez pecurka
					mapa[pecurka.row][pecurka.column] = PUT;
					pecurka.row = inf;
					pecurka.column = inf;
				}
				else{
					mapa[pocetak.row][pocetak.column] = PUT;
					pocetak.column--;
					mapa[pocetak.row][pocetak.column] = IGRAC;
					playerDistance++;
				}
			}
			else {
				printf("Ne moze tuda!\n");
				delay();
			}
			break;
		case RIGHT:
			//right
			if (mapa[pocetak.row][pocetak.column + 1] != ZID) {
				if (mapa[pocetak.row][pocetak.column + 1] == CILJ) {
					run = 0;
					win = 1;
					playerDistance++;
				}else{
					mapa[pocetak.row][pocetak.column] = PUT;
					pocetak.column++;
					mapa[pocetak.row][pocetak.column] = IGRAC;
					playerDistance++;
				}
			}
			else {
				printf("Ne moze tuda!\n");
				delay();
			}
			break;
		case DOWN:
			//down
			if (mapa[pocetak.row + 1][pocetak.column] != ZID) {
				if (mapa[pocetak.row + 1][pocetak.column] == CILJ) {
					run = 0;
					win = 1;
					playerDistance++;
				}else{
					mapa[pocetak.row][pocetak.column] = PUT;
					pocetak.row++;
					mapa[pocetak.row][pocetak.column] = IGRAC;
					playerDistance++;
				}
			}
			else {
				printf("Ne moze tuda!\n");
				delay();
			}
			break;
		case UP:
			//up
			if (mapa[pocetak.row - 1][pocetak.column] != ZID) {
				if (mapa[pocetak.row - 1][pocetak.column] == CILJ) {
					run = 0;
					win = 1;
					playerDistance++;
				}
				else {
					mapa[pocetak.row][pocetak.column] = PUT;
					pocetak.row--;
					mapa[pocetak.row][pocetak.column] = IGRAC;
					playerDistance++;
				}
			}
			else {
				printf("Ne moze tuda!\n");
				delay();
			}
			break;
	default:
		break;
	}
}
void game() {
	intro();
	showGameControls();
	while (run) {
		//prikazi mapu
		printMapu(mapa);
		//take keyboard
		switch (getch()) {
		case 'a':
			//left
			move(LEFT);
			//check if you got it
			break;
		case 'w':
			//up
			move(UP);
			break;
		case 's':
			//down
			move(DOWN);
			break;
		case 'd':
			//right
			move(RIGHT);
			break;
		case 'q':
			run = 0;
			break;
		}
		system("cls");
	}
	if (win) {
		showEndResults();
	}
}



int main(int argc, char* argv[]) {
	
	int ucitanFajl = ucitajMapuIzFajla(argc, argv); //1-ucitan; 0-nije ucitan
	if (!ucitanFajl) return 0;


	generisiMatricuGrafaNaOsnovuMape();
	inicijalizacijaDijkstrinogAlgoritma();
	DijkstrinAlgoritam();

	if (!proveraDaLiPostojiPutanjaOdTackeAdoTackeB(kraj.row * 6 + kraj.column)) {
		return 0;
	}

	if (!playGame) {
		putanjaOdTackeAdoTackeB(previous, indeksPocetka, kraj.row * 6 + kraj.column, mapa);
	}
	else {

		game();
	}



	return 0;
}