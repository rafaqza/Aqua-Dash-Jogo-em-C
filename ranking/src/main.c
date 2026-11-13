#include "raylib.h"
#include <stdlib.h>

#define LARGURA            800
#define ALTURA             600
#define CHAO               ALTURA

#define LARG_ALGA          60.0f
#define ESPACO_ALGAS       300.0f

#define VEL_INICIAL        3.0f
#define VEL_MAX            7.5f
#define ABERTURA_INICIAL   210.0f
#define ABERTURA_MIN       140.0f

typedef struct {
    Vector2 posicao;
    float velocidade_y;
    float gravidade;
    float pulo;
    float largura;
    float altura;
} Jogador;

typedef struct Alga {
    float x;
    float abertura_y;
    float abertura;
    int pontuada;
    struct Alga *proximo;
} Alga;

typedef struct {
    Alga *algas;
    int pontos;
    float velocidade;
    float abertura;
} Jogo;

/* ---------- JOGADOR ---------- */

void resetar_jogador(Jogador *p) {
    p->posicao.x = 150.0f;
    p->posicao.y = CHAO / 2.0f;
    p->velocidade_y = 0.0f;
}

void iniciar_jogador(Jogador *p) {
    p->gravidade = 0.4f;
    p->pulo = -7.5f;
    p->largura = 50.0f;
    p->altura = 40.0f;

    resetar_jogador(p);
}

void atualizar_jogador(Jogador *p, int pulou) {
    if (pulou) {
        p->velocidade_y = p->pulo;
    }

    p->velocidade_y += p->gravidade;
    p->posicao.y += p->velocidade_y;
}

// Caixa de colisão levemente reduzida para ser mais justa com o jogador
Rectangle hitbox_jogador(const Jogador *p) {
    return (Rectangle){
        p->posicao.x + 10.0f,
        p->posicao.y + 10.0f,
        p->largura - 20.0f,
        p->altura - 20.0f
    };
}

void desenhar_jogador(const Jogador *p) {
    DrawRectangle((int)p->posicao.x, (int)p->posicao.y,
                  (int)p->largura, (int)p->altura, ORANGE);
}

/* ---------- ALGAS (Lista Encadeada) ---------- */

Alga *ultima_alga(Alga *cabeca) {
    if (cabeca == NULL) return NULL;

    while (cabeca->proximo != NULL) {
        cabeca = cabeca->proximo;
    }

    return cabeca;
}

Alga *adicionar_alga(Jogo *j, float x) {
    Alga *nova = (Alga *)malloc(sizeof(Alga));
    if (nova == NULL) return NULL;

    float min_y = 70.0f;
    float max_y = CHAO - j->abertura - 70.0f;

    Alga *ultima = ultima_alga(j->algas);
    float y;

    if (ultima == NULL) {
        y = (float)GetRandomValue((int)min_y, (int)max_y);
    } else {
        y = ultima->abertura_y + (float)GetRandomValue(-170, 170);
    }

    if (y < min_y) y = min_y;
    if (y > max_y) y = max_y;

    nova->x = x;
    nova->abertura_y = y;
    nova->abertura = j->abertura;
    nova->pontuada = 0;
    nova->proximo = NULL;

    if (ultima == NULL) {
        j->algas = nova;
    } else {
        ultima->proximo = nova;
    }

    return nova;
}

void liberar_algas(Jogo *j) {
    Alga *atual = j->algas;

    while (atual != NULL) {
        Alga *prox = atual->proximo;
        free(atual);
        atual = prox;
    }

    j->algas = NULL;
}

void atualizar_algas(Jogo *j) {
    for (Alga *a = j->algas; a != NULL; a = a->proximo) {
        a->x -= j->velocidade;
    }

    // Remove algas que já saíram da tela pela esquerda
    while (j->algas != NULL && j->algas->x + LARG_ALGA < 0) {
        Alga *remover = j->algas;
        j->algas = remover->proximo;
        free(remover);
    }

    // Adiciona novas algas conforme necessário à direita
    Alga *ultima = ultima_alga(j->algas);

    while (ultima == NULL || ultima->x < LARGURA) {
        float x = (ultima == NULL) ? (float)LARGURA : ultima->x + ESPACO_ALGAS;

        Alga *nova = adicionar_alga(j, x);
        if (nova == NULL) break;

        ultima = nova;
    }
}

void desenhar_algas(const Jogo *j) {
    for (Alga *a = j->algas; a != NULL; a = a->proximo) {
        float base = a->abertura_y + a->abertura;

        DrawRectangle((int)a->x, 0, (int)LARG_ALGA, (int)a->abertura_y, GREEN);
        DrawRectangle((int)a->x, (int)base, (int)LARG_ALGA, (int)(CHAO - base), GREEN);
    }
}

/* ---------- JOGO ---------- */

void iniciar_jogo(Jogo *j) {
    j->algas = NULL;
}

void fim_de_jogo(Jogo *j) {
    liberar_algas(j);
}

void resetar_jogo(Jogo *j, Jogador *p) {
    liberar_algas(j);

    j->pontos = 0;
    j->velocidade = VEL_INICIAL;
    j->abertura = ABERTURA_INICIAL;

    resetar_jogador(p);
    atualizar_algas(j);
}

// Retorna 1 se o polvo encostou em alguma alga
int colidir(const Jogo *j, Rectangle hit) {
    for (Alga *a = j->algas; a != NULL; a = a->proximo) {
        float base = a->abertura_y + a->abertura;

        Rectangle cima = { a->x, 0.0f, LARG_ALGA, a->abertura_y };
        Rectangle baixo = { a->x, base, LARG_ALGA, CHAO - base };

        if (CheckCollisionRecs(hit, cima) || CheckCollisionRecs(hit, baixo)) {
            return 1;
        }
    }

    return 0;
}

// Roda um frame da partida; retorna 1 se o jogador morreu
int atualizar_jogo(Jogo *j, Jogador *p, int pulou) {
    atualizar_jogador(p, pulou);
    atualizar_algas(j);

    // Sistema de pontuação e aumento progressivo de dificuldade
    for (Alga *a = j->algas; a != NULL; a = a->proximo) {
        if (!a->pontuada && a->x + LARG_ALGA < p->posicao.x) {
            a->pontuada = 1;
            j->pontos++;

            j->velocidade = VEL_INICIAL + j->pontos * 0.2f;
            if (j->velocidade > VEL_MAX) {
                j->velocidade = VEL_MAX;
            }

            j->abertura = ABERTURA_INICIAL - j->pontos * 2.5f;
            if (j->abertura < ABERTURA_MIN) {
                j->abertura = ABERTURA_MIN;
            }
        }
    }

    Rectangle hit = hitbox_jogador(p);

    // Verifica colisão com o chão ou teto
    if (hit.y < 0 || hit.y + hit.height > CHAO) {
        return 1;
    }

    return colidir(j, hit);
}

int main(void) {
    InitWindow(LARGURA, ALTURA, "Polvo Pirata");
    SetTargetFPS(60);

    Jogador polvo;
    Jogo partida;

    iniciar_jogador(&polvo);
    iniciar_jogo(&partida);
    resetar_jogo(&partida, &polvo);

    int morreu = 0;

    while (!WindowShouldClose()) {
        int pulou = IsKeyPressed(KEY_SPACE) ||
                    IsKeyPressed(KEY_UP) ||
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (!morreu) {
            if (atualizar_jogo(&partida, &polvo, pulou)) {
                morreu = 1;
            }
        } else {
            if (IsKeyPressed(KEY_R)) {
                resetar_jogo(&partida, &polvo);
                morreu = 0;
            }
        }

        BeginDrawing();
            ClearBackground(SKYBLUE);

            desenhar_algas(&partida);
            desenhar_jogador(&polvo);

            DrawText(TextFormat("Pontos: %d", partida.pontos), 20, 15, 30, WHITE);

            if (morreu) {
                DrawText("GAME OVER", 280, 250, 50, RED);
                DrawText("Pressione R para reiniciar",
