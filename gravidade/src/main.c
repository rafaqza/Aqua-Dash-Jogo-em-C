#include "raylib.h"

#define LARGURA 800
#define ALTURA 600

typedef struct {
    Vector2 posicao;
    float velocidade_y;
    float gravidade;
    float largura;
    float altura;
} jogador;

void iniciar_jogador(jogador *p) {
    p->largura = 50.0f;
    p->altura = 40.0f;

    p->posicao.x = LARGURA / 2.0f - p->largura / 2.0f;
    p->posicao.y = ALTURA / 2.0f;
    p->velocidade_y = 0.0f;
    p->gravidade = 0.4f;
}

void atualizar_jogador(jogador *p) {
    p->velocidade_y += p->gravidade;
    p->posicao.y += p->velocidade_y;
}

void desenhar_jogador(jogador *p) {
    DrawRectangle((int)p->posicao.x,(int)p->posicao.y,(int)p->largura,(int)p->altura, ORANGE);
}

int main(void) {
    InitWindow(LARGURA,ALTURA,"AquaDash");
    SetTargetFPS(60);

    jogador polvo;
    iniciar_jogador(&polvo);

    while (!WindowShouldClose()) {
        atualizar_jogador(&polvo);

        if (polvo.posicao.y > ALTURA) {
            iniciar_jogador(&polvo);
        }

        BeginDrawing();
            ClearBackground(BLUE);
            desenhar_jogador(&polvo);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
