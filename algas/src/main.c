#include "raylib.h"
#include <stdlib.h>

#define LARGURA 800
#define ALTURA 600
#define CHAO ALTURA

#define LARG_ALGA 60.0f
#define ESPACO_ALGAS 300.0f

#define VEL_INICIAL 3.0f
#define ABERTURA_INICIAL 210.0f

typedef struct {
    Vector2 posicao;
    float velocidade_y;
    float gravidade;
    float pulo;
    float largura;
    float altura;
} jogador;

/* cada alga e um no da lista encadeada */
typedef struct alga {
    float x;
    float abertura_y;
    float abertura;
    struct alga *proximo;
} alga;

typedef struct {
    alga *algas;
    float velocidade;
    float abertura;
} jogo;

/* ---------- jogador ---------- */

void resetar_jogador(jogador *p) {
    p->posicao.x = 150.0f;
    p->posicao.y = CHAO / 2.0f;
    p->velocidade_y = 0.0f;
}

void iniciar_jogador(jogador *p) {
    p->gravidade = 0.4f;
    p->pulo = -7.5f;
    p->largura = 50.0f;
    p->altura = 40.0f;

    resetar_jogador(p);
}

void atualizar_jogador(jogador *p, int pulou) {
    if (pulou) {
        p->velocidade_y = p->pulo;
    }

    p->velocidade_y += p->gravidade;
    p->posicao.y += p->velocidade_y;
}

int bateu_nos_limites(jogador *p) {
    if (p->posicao.y < 0 || p->posicao.y + p->altura > CHAO) {
        return 1;
    }

    return 0;
}

void desenhar_jogador(jogador *p) {
    DrawRectangle((int)p->posicao.x, (int)p->posicao.y,
                  (int)p->largura, (int)p->altura, ORANGE);
}

/* ---------- algas (lista encadeada) ---------- */

/* anda pela lista ate achar a ultima alga */
alga *ultima_alga(alga *cabeca) {
    if (cabeca == NULL) return NULL;

    while (cabeca->proximo != NULL) {
        cabeca = cabeca->proximo;
    }

    return cabeca;
}

/* cria uma alga nova com malloc e coloca no fim da lista */
alga *adicionar_alga(jogo *j, float x) {
    alga *nova = (alga *)malloc(sizeof(alga));

    if (nova == NULL) return NULL;

    float min_y = 70.0f;
    float max_y = CHAO - j->abertura - 70.0f;

    alga *ultima = ultima_alga(j->algas);
    float y;

    if (ultima == NULL) {
        y = (float)GetRandomValue((int)min_y, (int)max_y);
    } else {
        /* perto da alga anterior, pra sempre dar pra passar */
        y = ultima->abertura_y + (float)GetRandomValue(-170, 170);
    }

    if (y < min_y) y = min_y;
    if (y > max_y) y = max_y;

    nova->x = x;
    nova->abertura_y = y;
    nova->abertura = j->abertura;
    nova->proximo = NULL;

    if (ultima == NULL) {
        j->algas = nova;
    } else {
        ultima->proximo = nova;
    }

    return nova;
}

/* todo malloc precisa de um free */
void liberar_algas(jogo *j) {
    alga *atual = j->algas;

    while (atual != NULL) {
        alga *prox = atual->proximo;
        free(atual);
        atual = prox;
    }

    j->algas = NULL;
}

void atualizar_algas(jogo *j) {
    alga *a;

    for (a = j->algas; a != NULL; a = a->proximo) {
        a->x -= j->velocidade;
    }

    /* remove do comeco da lista as que sairam da tela */
    while (j->algas != NULL && j->algas->x + LARG_ALGA < 0) {
        alga *remover = j->algas;
        j->algas = remover->proximo;
        free(remover);
    }

    /* cria algas novas ate encher o lado direito da tela */
    alga *ultima = ultima_alga(j->algas);

    while (ultima == NULL || ultima->x < LARGURA) {
        float x;

        if (ultima == NULL) {
            x = (float)LARGURA;
        } else {
            x = ultima->x + ESPACO_ALGAS;
        }

        alga *nova = adicionar_alga(j, x);

        if (nova == NULL) break;

        ultima = nova;
    }
}

void desenhar_algas(jogo *j) {
    alga *a;

    for (a = j->algas; a != NULL; a = a->proximo) {
        float base = a->abertura_y + a->abertura;

        /* parte de cima e parte de baixo */
        DrawRectangle((int)a->x, 0, (int)LARG_ALGA, (int)a->abertura_y, GREEN);
        DrawRectangle((int)a->x, (int)base, (int)LARG_ALGA, (int)(CHAO - base), GREEN);
    }
}

/* ---------- jogo ---------- */

void iniciar_jogo(jogo *j) {
    j->algas = NULL;
}

void fim_de_jogo(jogo *j) {
    liberar_algas(j);
}

void resetar_jogo(jogo *j, jogador *p) {
    liberar_algas(j);

    j->velocidade = VEL_INICIAL;
    j->abertura = ABERTURA_INICIAL;

    resetar_jogador(p);
    atualizar_algas(j);
}

int main(void) {
    InitWindow(LARGURA, ALTURA, "AquaDash");
    SetTargetFPS(60);

    jogador polvo;
    jogo partida;

    iniciar_jogador(&polvo);
    iniciar_jogo(&partida);
    resetar_jogo(&partida, &polvo);

    int morreu = 0;

    while (!WindowShouldClose()) {
        int pulou = IsKeyPressed(KEY_SPACE);

        if (!morreu) {
            atualizar_jogador(&polvo, pulou);
            atualizar_algas(&partida);

            if (bateu_nos_limites(&polvo)) {
                morreu = 1;
            }
        } else {
            if (IsKeyPressed(KEY_R)) {
                resetar_jogo(&partida, &polvo);
                morreu = 0;
            }
        }

        BeginDrawing();
            ClearBackground(BLUE);
            desenhar_algas(&partida);
            desenhar_jogador(&polvo);

            if (morreu) {
                DrawText("GAME OVER", 280, 250, 50, RED);
                DrawText("Pressione R para reiniciar", 270, 320, 22, DARKGRAY);
            }
        EndDrawing();
    }

    fim_de_jogo(&partida);
    CloseWindow();

    return 0;
}
