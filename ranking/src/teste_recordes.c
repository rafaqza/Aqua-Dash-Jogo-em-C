/* programa de teste SO do ranking (nao usa raylib).
 * serve pra testar recordes.c antes de ligar no jogo. */
#include <stdio.h>
#include "recordes.h"

void mostrar(int recordes[]) {
    int i;

    printf("--- TOP SCORES ---\n");

    for (i = 0; i < MAX_RECORDES; i++) {
        printf("%d. %d\n", i + 1, recordes[i]);
    }
}

int main(void) {
    int recordes[MAX_RECORDES];
    int pontos;

    carregar_recordes(recordes);
    mostrar(recordes);

    printf("\ndigite uma pontuacao: ");
    scanf("%d", &pontos);

    if (entra_no_ranking(recordes, pontos)) {
        inserir_recorde(recordes, pontos);
        salvar_recordes(recordes);
        printf("entrou no ranking!\n\n");
    } else {
        printf("nao entrou no ranking.\n\n");
    }

    mostrar(recordes);

    return 0;
}
