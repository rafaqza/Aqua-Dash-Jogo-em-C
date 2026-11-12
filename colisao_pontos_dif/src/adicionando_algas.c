#include "raylib.h"
#include <stdlib.h>

// Configurações básicas da tela do jogo
#define LARGURA 800
#define ALTURA 600
#define CHAO ALTURA

// Configurações dos obstáculos (algas)
#define LARG_ALGA 60.0f
#define ESPACO_ALGAS 300.0f

// Dificuldade e velocidade inicial
#define VEL_INICIAL 3.0f
#define VEL_MAX 7.5f
#define ABERTURA_INICIAL 210.0f
#define ABERTURA_MIN 140.0f

// Estrutura que define o nosso personagem (o polvo)
typedef struct {
    Vector2 posicao;
    float velocidade_y;
    float gravidade;
    float pulo;
    float largura;
    float altura;
} jogador;

// Estrutura para cada alga usando lista encadeada
typedef struct alga {
    float x;
    float abertura_y;
    float abertura;
    int pontuada;
    struct alga *proximo;
} alga;

// Estrutura geral que controla o estado do jogo
typedef struct {
    alga *algas;
    int pontos;
    float velocidade;
    float abertura;
} jogo;

// --- FUNÇÕES DO JOGADOR ---

// Reseta a posição do polvo para o centro da tela
void resetar_jogador(jogador *p) {
    p->posicao.x = 150.0f;
    p->posicao.y = CHAO / 2.0f;
    p->velocidade_y = 0.0f;
}

// Configura os atributos físicos iniciais do polvo
void iniciar_jogador(jogador *p) {
    p->gravidade = 0.4f;
    p->pulo = -7.5f;
    p->largura = 50.0f;
    p->altura = 40.0f;
    resetar_jogador(p);
}

// Atualiza a movimentação e a gravidade do polvo a cada frame
void atualizar_jogador(jogador *p, int pulou) {
    if (pulou) {
        p->velocidade_y = p->pulo; // Dá o impulso para cima
    }
    p->velocidade_y += p->gravidade; // Puxa o polvo para baixo
    p->posicao.y += p->velocidade_y;
}

// Desenha o polvo na tela
void desenhar_jogador(jogador *p) {
    DrawRectangle((int)p->posicao.x, (int)p->posicao.y,
                  (int)p->largura, (int)p->altura, ORANGE);
}

// --- FUNÇÕES DAS ALGAS (OBSTÁCULOS) ---

// Encontra a última alga da lista encadeada para sabermos onde posicionar a próxima
alga *ultima_alga(alga *cabeca) {
    if (cabeca == NULL) return NULL;
    while (cabeca->proximo != NULL) {
        cabeca = cabeca->proximo;
    }
    return cabeca;
}

// Cria e adiciona uma nova alga no cenário
alga *adicionar_alga(jogo *j, float x) {
    alga *nova = (alga *)malloc(sizeof(alga));
    if (nova == NULL) return NULL;

    float min_y = 70.0f;
    float max_y = CHAO - j->abertura - 70.0f;
    alga *ultima = ultima_alga(j->algas);
    float y;

    // Define a altura da abertura da alga de forma procedural
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

// Limpa a memória RAM liberando todas as algas criadas
void liberar_algas(jogo *j) {
    alga *atual = j->algas;
    while (atual != NULL) {
        alga *prox = atual->proximo;
        free(atual);
        atual = prox;
    }
    j->algas = NULL;
}

// Move as algas para a esquerda e recicla as que já saíram da tela
void atualizar_algas(jogo *j) {
    alga *a;
    for (a = j->algas; a != NULL; a = a->proximo) {
        a->x -= j->velocidade;
    }

    // Remove algas que saíram da tela pela esquerda
    while (j->algas != NULL && j->algas->x + LARG_ALGA < 0) {
        alga *remover = j->algas;
        j->algas = remover->proximo;
        free(remover);
    }

    // Garante que sempre haverá novas algas surgindo à direita
    alga *ultima = ultima_alga(j->algas);
    while (ultima == NULL || ultima->x < LARGURA) {
        float x = (ultima == NULL) ? (float)LARGURA : ultima->x + ESPACO_ALGAS;
        alga *nova = adicionar_alga(j, x);
        if (nova == NULL) break;
        ultima = nova;
    }
}

// Desenha a parte de cima e de baixo das algas na tela
void desenhar_algas(jogo *j) {
    alga *a;
    for (a = j->algas; a != NULL; a = a->proximo) {
        float base = a->abertura_y + a->abertura;
        DrawRectangle((int)a->x, 0, (int)LARG_ALGA, (int)a->abertura_y, GREEN);
        DrawRectangle((int)a->x, (int)base, (int)LARG_ALGA, (int)(CHAO - base), GREEN);
    }
}

// --- LOOP PRINCIPAL DO JOGO ---

int main(void) {
    InitWindow(LARGURA, ALTURA, "AquaDash");
    SetTargetFPS(60);

    jogador polvo;
    jogo partida = {0};

    // Inicializa os elementos
    iniciar_jogador(&polvo);
    partida.velocidade = VEL_INICIAL;
    partida.abertura = ABERTURA_INICIAL;
    atualizar_algas(&partida);

    // Laço principal do jogo roda a 60 quadros por segundo
    while (!WindowShouldClose()) {
        int pulou = IsKeyPressed(KEY_SPACE);

        // Atualiza posições
        atualizar_jogador(&polvo, pulou);
        atualizar_algas(&partida);

        // Renderização gráfica
        BeginDrawing();
            ClearBackground(BLUE); // Fundo azul simulando o oceano
            desenhar_algas(&partida);
            desenhar_jogador(&polvo);
        EndDrawing();
    }

    // Limpeza de memória antes de fechar o programa
    liberar_algas(&partida);
    CloseWindow();
    return 0;
}
