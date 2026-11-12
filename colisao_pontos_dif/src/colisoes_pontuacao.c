#include "raylib.h"
#include <stdlib.h>

// Configurações da tela do jogo
#define LARGURA 800
#define ALTURA 600
#define CHAO ALTURA

// Tamanho e espaçamento das algas (obstáculos)
#define LARG_ALGA 60.0f
#define ESPACO_ALGAS 300.0f

// Parâmetros de dificuldade e progressão
#define VEL_INICIAL 3.0f
#define VEL_MAX 7.5f
#define ABERTURA_INICIAL 210.0f
#define ABERTURA_MIN 140.0f

// Estrutura do jogador (o polvo)
typedef struct {
    Vector2 posicao;
    float velocidade_y;
    float gravidade;
    float pulo;
    float largura;
    float altura;
} jogador;

// Estrutura de cada alga usando lista encadeada
typedef struct alga {
    float x;
    float abertura_y;
    float abertura;
    int pontuada;
    struct alga *proximo;
} alga;

// Estrutura principal que gerencia o estado da partida
typedef struct {
    alga *algas;
    int pontos;
    float velocidade;
    float abertura;
} jogo;

/* ---------- FUNÇÕES DO JOGADOR ---------- */

// Reseta a posição do polvo para o meio da tela
void resetar_jogador(jogador *p) {
    p->posicao.x = 150.0f;
    p->posicao.y = CHAO / 2.0f;
    p->velocidade_y = 0.0f;
}

// Configura os valores iniciais de física do polvo
void iniciar_jogador(jogador *p) {
    p->gravidade = 0.4f;
    p->pulo = -7.5f;
    p->largura = 50.0f;
    p->altura = 40.0f;

    resetar_jogador(p);
}

// Atualiza a gravidade e o pulo do polvo a cada frame
void atualizar_jogador(jogador *p, int pulou) {
    if (pulou) {
        p->velocidade_y = p->pulo;
    }

    p->velocidade_y += p->gravidade;
    p->posicao.y += p->velocidade_y;
}

// Cria uma caixa de colisão levemente menor que o polvo para o jogo ser mais justo
Rectangle hitbox_jogador(jogador *p) {
    Rectangle r = {
        p->posicao.x + 10.0f,
        p->posicao.y + 10.0f,
        p->largura - 20.0f,
        p->altura - 20.0f
    };

    return r;
}

// Desenha o polvo na tela
void desenhar_jogador(jogador *p) {
    DrawRectangle((int)p->posicao.x, (int)p->posicao.y,
                  (int)p->largura, (int)p->altura, ORANGE);
}

/* ---------- FUNÇÕES DAS ALGAS ---------- */

// Acha a última alga da lista encadeada para sabermos onde posicionar a próxima
alga *ultima_alga(alga *cabeca) {
    if (cabeca == NULL) return NULL;

    while (cabeca->proximo != NULL) {
        cabeca = cabeca->proximo;
    }

    return cabeca;
}

// Cria e adiciona um novo obstáculo de algas no cenário
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

// Limpa toda a memória das algas ao reiniciar ou fechar o jogo
void liberar_algas(jogo *j) {
    alga *atual = j->algas;

    while (atual != NULL) {
        alga *prox = atual->proximo;
        free(atual);
        atual = prox;
    }

    j->algas = NULL;
}

// Move as algas para a esquerda e recicla as que saem da tela
void atualizar_algas(jogo *j) {
    alga *a;

    for (a = j->algas; a != NULL; a = a->proximo) {
        a->x -= j->velocidade;
    }

    // Remove algas antigas da memória quando saem da tela
    while (j->algas != NULL && j->algas->x + LARG_ALGA < 0) {
        alga *remover = j->algas;
        j->algas = remover->proximo;
        free(remover);
    }

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

// Desenha a parte de cima e de baixo de cada alga
void desenhar_algas(jogo *j) {
    alga *a;

    for (a = j->algas; a != NULL; a = a->proximo) {
        float base = a->abertura_y + a->abertura;

        DrawRectangle((int)a->x, 0, (int)LARG_ALGA, (int)a->abertura_y, GREEN);
        DrawRectangle((int)a->x, (int)base, (int)LARG_ALGA, (int)(CHAO - base), GREEN);
    }
}

/* ---------- LÓGICA DO JOGO ---------- */

void iniciar_jogo(jogo *j) {
    j->algas = NULL;
}

void fim_de_jogo(jogo *j) {
    liberar_algas(j);
}

// Reseta pontuação, velocidade e reinicia a partida
void resetar_jogo(jogo *j, jogador *p) {
    liberar_algas(j);

    j->pontos = 0;
    j->velocidade = VEL_INICIAL;
    j->abertura = ABERTURA_INICIAL;

    resetar_jogador(p);
    atualizar_algas(j);
}

// Verifica se o polvo bateu em alguma alga
int colidir(jogo *j, Rectangle hit) {
    alga *a;

    for (a = j->algas; a != NULL; a = a->proximo) {
        float base = a->abertura_y + a->abertura;

        Rectangle cima = { a->x, 0.0f, LARG_ALGA, a->abertura_y };
        Rectangle baixo = { a->x, base, LARG_ALGA, CHAO - base };

        if (CheckCollisionRecs(hit, cima) || CheckCollisionRecs(hit, baixo)) {
            return 1; // Colidiu
        }
    }

    return 0; // Sem colisão
}

// Atualiza a lógica de pontuação, dificuldade progressiva e colisões a cada frame
int atualizar_jogo(jogo *j, jogador *p, int pulou) {
    alga *a;

    atualizar_jogador(p, pulou);
    atualizar_algas(j);

    // Sistema de pontuação e aumento progressivo da dificuldade
    for (a = j->algas; a != NULL; a = a->proximo) {
        if (!a->pontuada && a->x + LARG_ALGA < p->posicao.x) {
            a->pontuada = 1;
            j->pontos++;

            // Fica mais rápido conforme ganha pontos
            j->velocidade = VEL_INICIAL + j->pontos * 0.2f;
            if (j->velocidade > VEL_MAX) {
                j->velocidade = VEL_MAX;
            }

            // O espaço entre as algas diminui para dificultar
            j->abertura = ABERTURA_INICIAL - j->pontos * 2.5f;
            if (j->abertura < ABERTURA_MIN) {
                j->abertura = ABERTURA_MIN;
            }
        }
    }

    Rectangle hit = hitbox_jogador(p);

    // Morre se bater no chão ou no teto da tela
    if (hit.y < 0 || hit.y + hit.height > CHAO) {
        return 1;
    }

    return colidir(j, hit);
}

// --- FUNÇÃO PRINCIPAL ---

int main(void) {
    InitWindow(LARGURA, ALTURA, "AquaDash");
    SetTargetFPS(60);

    jogador polvo;
    jogo partida;

    iniciar_jogador(&polvo);
    iniciar_jogo(&partida);
    resetar_jogo(&partida, &polvo);

    int morreu = 0;

    // Loop principal do jogo
    while (!WindowShouldClose()) {
        int pulou = IsKeyPressed(KEY_SPACE);

        if (!morreu) {
            if (atualizar_jogo(&partida, &polvo, pulou)) {
                morreu = 1;
            }
        } else {
            // Se morreu, aguarda o jogador pressionar 'R' para reiniciar
            if (IsKeyPressed(KEY_R)) {
                resetar_jogo(&partida, &polvo);
                morreu = 0;
            }
        }

        // Renderização gráfica
        BeginDrawing();
            ClearBackground(BLUE); // Fundo do oceano
            desenhar_algas(&partida);
            desenhar_jogador(&polvo);

            // Exibe a pontuação na tela
            DrawText(TextFormat("Pontos: %d", partida.pontos), 20, 15, 30, WHITE);

            // Mensagens de Game Over
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
