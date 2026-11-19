/* Programa de teste do ranking (sem Raylib).
 * Útil para testar recordes.c antes de integrar ao jogo. */
#include <stdio.h>
#include "recordes.h"

// Exibe o ranking formatado
void mostrar(const int recordes[]) {
    printf("\n--- TOP SCORES ---\n");
    for (int i = 0; i < MAX_RECORDES; i++) {
        printf("%2d. %d\n", i + 1, recordes[i]);
    }
    printf("------------------\n");
}

int main(void) {
    int recordes[MAX_RECORDES];
    int pontos;

    carregar_recordes(recordes);

    printf("=== TESTE DO SISTEMA DE RECORDES ===\n");

    // Loop para testar várias pontuações sem precisar reiniciar o programa
    while (1) {
        mostrar(recordes);

        printf("\nDigite uma pontuacao (ou um valor negativo para sair): ");
        if (scanf("%d", &pontos) != 1) {
            printf("Entrada invalida. Encerrando...\n");
            break;
        }

        if (pontos < 0) {
            printf("Saindo do teste de recordes. Ate logo!\n");
            break;
        }

        // Verifica, insere e salva se entrou no ranking
        if (entra_no_ranking(recordes, pontos)) {
            inserir_recorde(recordes, pontos);
            salvar_recordes(recordes);
            printf("\n[Sucesso] Entrou no ranking!\n");
        } else {
            printf("\n[Info] Nao entrou no ranking.\n");
        }
    }

    return 0;
}
