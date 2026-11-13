#include "recordes.h"
#include <stdio.h>

void carregar_recordes(int recordes[]) {
    // Inicializa o array com zeros por padrão
    for (int i = 0; i < MAX_RECORDES; i++) {
        recordes[i] = 0;
    }

    FILE *arquivo = fopen(ARQUIVO_RECORDES, "r");
    if (arquivo == NULL) {
        return; // Se o arquivo não existe, mantém os zeros
    }

    for (int i = 0; i < MAX_RECORDES; i++) {
        if (fscanf(arquivo, "%d", &recordes[i]) != 1) {
            break; // Para se o arquivo acabar antes do esperado
        }
    }

    fclose(arquivo);
}

void salvar_recordes(const int recordes[]) {
    FILE *arquivo = fopen(ARQUIVO_RECORDES, "w");
    if (arquivo == NULL) {
        return; // Erro ao abrir arquivo para escrita
    }

    for (int i = 0; i < MAX_RECORDES; i++) {
        fprintf(arquivo, "%d\n", recordes[i]);
    }

    fclose(arquivo);
}

int entra_no_ranking(const int recordes[], int pontos) {
    // Retorna 1 se a pontuação for maior que a última posição do ranking
    return (pontos > recordes[MAX_RECORDES - 1]);
}

void inserir_recorde(int recordes[], int pontos) {
    int i = MAX_RECORDES - 1;

    recordes[i] = pontos;

    // Organiza o ranking em ordem decrescente (bubble/insertion swap)
    while (i > 0 && recordes[i] > recordes[i - 1]) {
        int temporario = recordes[i];
        recordes[i] = recordes[i - 1];
        recordes[i - 1] = temporario;
        i--;
    }
}
