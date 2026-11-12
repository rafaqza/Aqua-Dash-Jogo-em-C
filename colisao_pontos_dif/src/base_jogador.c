#include "raylib.h"

// Configurações básicas da tela do jogo
#define LARGURA 800
#define ALTURA 600
#define CHAO ALTURA

// Estrutura que define as propriedades físicas e visuais do nosso jogador (o polvo)
typedef struct {
    Vector2 posicao;
    float velocidade_y;
    float gravidade;
    float pulo;
    float largura;
    float altura;
} jogador;

// Reseta a posição do polvo para o meio da tela
void resetar_jogador(jogador *p) {
    p->posicao.x = 150.0f;
    p->posicao.y = CHAO / 2.0f;
    p->velocidade_y = 0.0f;
}

// Configura os valores iniciais de física do polvo (gravidade e força do pulo)
void iniciar_jogador(jogador *p) {
    p->gravidade = 0.4f;
    p->pulo = -7.5f;
    p->largura = 50.0f;
    p->altura = 40.0f;

    resetar_jogador(p);
}

// Atualiza a movimentação do polvo a cada frame (lida com o pulo e a queda por gravidade)
void atualizar_jogador(jogador *p, int pulou) {
    if (pulou) {
        p->velocidade_y = p->pulo; // Joga o polvo para cima
    }

    p->velocidade_y += p->gravidade; // Puxa o polvo para baixo constantemente
    p->posicao.y += p->velocidade_y;
}

// Desenha o polvo na tela como um bloco laranja
void desenhar_jogador(jogador *p) {
    DrawRectangle((int)p->posicao.x, (int)p->posicao.y,
                  (int)p->largura, (int)p->altura, ORANGE);
}

// --- LOOP PRINCIPAL ---

int main(void) {
    InitWindow(LARGURA, ALTURA, "AquaDash");
    SetTargetFPS(60);

    jogador polvo;
    iniciar_jogador(&polvo);

    // Laço principal do jogo (roda a 60 quadros por segundo)
    while (!WindowShouldClose()) {
        // Verifica se o jogador apertou a barra de espaço para pular
        int pulou = IsKeyPressed(KEY_SPACE);
        atualizar_jogador(&polvo, pulou);

        // Renderização na tela
        BeginDrawing();
            ClearBackground(BLUE); // Fundo azul simulando o oceano
            desenhar_jogador(&polvo);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
