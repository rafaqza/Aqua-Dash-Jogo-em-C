
#include "recordes.h"
#include <stdio.h>

void carregar_recordes(int recordes[]) {
    int i;

    for (i = 0; i < MAX_RECORDES; i++) {
        recordes[i] = 0;
    }

    FILE *arquivo = fopen(ARQUIVO_RECORDES, "r");

    if (arquivo == NULL) {
        return;
    }

    for (i = 0; i < MAX_RECORDES; i++) {
        if (fscanf(arquivo, "%d", &recordes[i]) != 1) {
            break;
        }
    }

    fclose(arquivo);
}

void salvar_recordes(int recordes[]) {
    int i;

    FILE *arquivo = fopen(ARQUIVO_RECORDES, "w");

    if (arquivo == NULL) {
        return;
    }

    for (i = 0; i < MAX_RECORDES; i++) {
        fprintf(arquivo, "%d\n", recordes[i]);
    }

    fclose(arquivo);
}

int entra_no_ranking(int recordes[], int pontos) {
    if (pontos > recordes[MAX_RECORDES - 1]) {
        return 1;
    }

    return 0;
}

void inserir_recorde(int recordes[], int pontos) {
    int i = MAX_RECORDES - 1;

    recordes[i] = pontos;

    while (i > 0 && recordes[i] > recordes[i - 1]) {
        int temporario = recordes[i];

        recordes[i] = recordes[i - 1];
        recordes[i - 1] = temporario;

        i--;
    }
}
