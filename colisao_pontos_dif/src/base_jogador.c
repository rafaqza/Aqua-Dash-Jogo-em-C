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

void desenhar_jogador(jogador *p) {
    DrawRectangle((int)p->posicao.x, (int)p->posicao.y,
                  (int)p->largura, (int)p->altura, ORANGE);
}

int main(void) {
    InitWindow(LARGURA, ALTURA, "AquaDash");
    SetTargetFPS(60);

    jogador polvo;
    iniciar_jogador(&polvo);

    while (!WindowShouldClose()) {
        int pulou = IsKeyPressed(KEY_SPACE);
        atualizar_jogador(&polvo, pulou);

        BeginDrawing();
            ClearBackground(BLUE);
            desenhar_jogador(&polvo);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
