#include "raylib.h"

#define LARGURA 800
#define ALTURA 600
#define CHAO ALTURA

typedef struct {
    Vector2 posicao;
    float velocidade_y;
    float gravidade;
    float pulo;
    float largura;
    float altura;
} jogador;

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
    DrawRectangle((int)p->posicao.x,(int)p->posicao.y,(int)p->largura,(int)p->altura, ORANGE);
}

int main(void) {
    InitWindow(LARGURA, ALTURA, "AquaDash");
    SetTargetFPS(60);

    jogador polvo;
    iniciar_jogador(&polvo);

    int morreu = 0;

    while (!WindowShouldClose()) {
        int pulou = IsKeyPressed(KEY_SPACE);

        if (!morreu) {
            atualizar_jogador(&polvo, pulou);

            if (bateu_nos_limites(&polvo)) {
                morreu = 1;
            }
        } else {
            if (IsKeyPressed(KEY_R)) {
                resetar_jogador(&polvo);
                morreu = 0;
            }
        }

        BeginDrawing();
            ClearBackground(BLUE);
            desenhar_jogador(&polvo);

            if (morreu) {
                DrawText("GAME OVER", 280, 250, 50, RED);
                DrawText("Pressione R para reiniciar", 270, 320, 22, DARKGRAY);
            }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
