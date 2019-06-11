#include <stdio.h>
#include <stdlib.h>
#include "omp.h"
#include "string.h"

#define N 50000

// /home/jonas/Documentos/arq.txt

void sequencial(char * substring, char * texto, int subSize);
void paralelo(char * substring, char * texto, int subSize);

int main() {
    
    char path[60], substring[15];
    int fileSize;

    printf("Insira o caminho do arquivo: ");
    scanf("%s", path);

    printf("Qual palavra deseja procurar? ");
    scanf("%s", substring);
    printf(" \n ============= RESULTADOS =============\n\n");

    int subSize = strlen(substring);

    FILE *file = fopen(path, "r");

    fseek(file, 0, SEEK_END);

    fileSize = ftell(file); // pegar o tamanho do arquivo pra alocar a variável

    fseek(file, 0, SEEK_SET);

    char * texto = (char*)malloc(sizeof(char)*fileSize);

	
    if(file != NULL) {

        fread(texto, fileSize, 1, file); // ler o arquivo e guardar na variavel do primeiro parametro
        sequencial(substring, texto, subSize);
        paralelo(substring, texto, subSize);

    } else {
        printf("Erro ao abrir o arquivo.\n");
    }

    free(texto);
}

void sequencial(char * substring, char * texto, int subSize) {

    int i = 0, j;
    double inicio, fim;

    inicio = omp_get_wtime();

    for(j = 0; j < strlen(texto) && i != subSize; j++) {
        if(texto[j] == substring[i]) i++;
        else i = 0;

        if(i == subSize) printf("Palavra Encontrada (Sequencial)!\n");
    }

    printf("Tempo Sequencial: %f\n\n", omp_get_wtime() - inicio);
}

void paralelo(char * substring, char * texto, int subSize) {
    
    double inicio, fim;
    int stop = 0;

    inicio = omp_get_wtime();

    omp_set_num_threads(4);

    #pragma omp parallel shared(stop)
    {
        int i = 0, j = 0, k = 0;
        int id = omp_get_thread_num();
        int N_THREAD = omp_get_num_threads();  
  
	//for(j = id * N_THREAD; j < strlen(texto) && !stop; j += N_THREAD) {
        for(j = id * subSize; j < strlen(texto) && !stop; j += N_THREAD) {

            if(texto[j] == substring[0]){
                i++;
                
                for(k = j+1; k < subSize + j; k++) {
                    //printf("%c", texto[k]);
                    if(texto[k] == substring[i]) i++;
                    else i = 0;
                }
   	        
	        
                if(i == subSize && !stop) {
                    printf("Palavra Encontrada pela thread = %d!! (Paralelo)", id);
		    //#pragma omp critical
                    stop = 1;
                }
            }
        }
    }

    printf("\nTempo Paralelo: %f\n", omp_get_wtime() - inicio);  
}
