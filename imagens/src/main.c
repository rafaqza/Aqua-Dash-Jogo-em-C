#include "raylib.h"
#include <stdlib.h>

#define LARGURA 800
#define ALTURA 600
#define ALT_CHAO 60
#define CHAO (ALTURA - ALT_CHAO)

#define LARG_ALGA 140.0f
#define ALT_ALGA 400.0f
#define LARG_HITBOX_ALGA 50.0f
#define MARGEM_ALGA 15.0f
#define ESPACO_ALGAS 300.0f

#define VEL_INICIAL 3.0f
#define VEL_MAX 7.5f
#define ABERTURA_INICIAL 210.0f
#define ABERTURA_MIN 140.0f

#define TAM_TILE 20
#define LINHAS_FUNDO 30
#define COLUNAS_FUNDO 40

typedef struct {
    Vector2 posicao;
    float velocidade_y;
    float gravidade;
    float pulo;
    float largura;
    float altura;
    Texture2D textura;
} jogador;

typedef struct alga {
    float x;
    float abertura_y;
    float abertura;
    int pontuada;
    struct alga *proximo;
} alga;

typedef struct {
    alga *algas;
    int fundo[LINHAS_FUNDO][COLUNAS_FUNDO];
    Texture2D tex_alga;
    Texture2D tex_areia;
    int pontos;
    float velocidade;
    float abertura;
} jogo;

void resetar_jogador(jogador *p) {
    p->posicao.x = 150.0f;
    p->posicao.y = CHAO / 2.0f;
    p->velocidade_y = 0.0f;
}

void iniciar_jogador(jogador *p) {
    p->textura = LoadTexture("assets/polvo_pirata.png");
    SetTextureFilter(p->textura, TEXTURE_FILTER_BILINEAR);

    p->altura = 72.0f;
    p->largura = 72.0f * p->textura.width / p->textura.height;
    p->gravidade = 0.4f;
    p->pulo = -7.5f;

    resetar_jogador(p);
}

void atualizar_jogador(jogador *p, int pulou) {
    if (pulou) {
        p->velocidade_y = p->pulo;
    }

    p->velocidade_y += p->gravidade;
    p->posicao.y += p->velocidade_y;
}

Rectangle hitbox_jogador(jogador *p) {
    Rectangle r = {
        p->posicao.x + 10.0f,
        p->posicao.y + 10.0f,
        p->largura - 20.0f,
        p->altura - 20.0f
    };

    return r;
}

void desenhar_jogador(jogador *p) {

    float rot = p->velocidade_y * 3.0f;

    if (rot < -25.0f) rot = -25.0f;
    if (rot > 35.0f) rot = 35.0f;

    Rectangle src = {
        0, 0,
        (float)p->textura.width,
        (float)p->textura.height
    };

    Rectangle dst = {
        p->posicao.x + p->largura / 2.0f,
        p->posicao.y + p->altura / 2.0f,
        p->largura,
        p->altura
    };

    Vector2 origem = {
        p->largura / 2.0f,
        p->altura / 2.0f
    };

    DrawTexturePro(p->textura, src, dst, origem, rot, WHITE);
}

alga *ultima_alga(alga *cabeca) {
    if (cabeca == NULL) return NULL;

    while (cabeca->proximo != NULL) {
        cabeca = cabeca->proximo;
    }

    return cabeca;
}

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

void desenhar_algas(jogo *j) {
    float w = (float)j->tex_alga.width;
    float h = (float)j->tex_alga.height;

    Vector2 origem = { 0, 0 };

    alga *a;

    for (a = j->algas; a != NULL; a = a->proximo) {
        Rectangle src_cima = { 0, 0, w, -h };
        Rectangle dst_cima = {
            a->x,
            a->abertura_y - ALT_ALGA,
            LARG_ALGA,
            ALT_ALGA
        };

        DrawTexturePro(j->tex_alga, src_cima, dst_cima, origem, 0.0f, WHITE);

        Rectangle src_baixo = { 0, 0, w, h };
        Rectangle dst_baixo = {
            a->x,
            a->abertura_y + a->abertura,
            LARG_ALGA,
            ALT_ALGA
        };

        DrawTexturePro(j->tex_alga, src_baixo, dst_baixo, origem, 0.0f, WHITE);
    }
}

void iniciar_fundo(jogo *j) {
    int i, k;

    for (i = 0; i < LINHAS_FUNDO; i++) {
        for (k = 0; k < COLUNAS_FUNDO; k++) {
            j->fundo[i][k] = GetRandomValue(-1, 1);
        }
    }
}

void desenhar_fundo(jogo *j) {
    int i, k;

    for (i = 0; i < LINHAS_FUNDO; i++) {
        float t = i / (float)(LINHAS_FUNDO - 1);

        for (k = 0; k < COLUNAS_FUNDO; k++) {
            int tom = j->fundo[i][k] * 3;

            int r = (int)(40 - 35 * t) + tom;
            int g = (int)(150 - 115 * t) + tom;
            int b = (int)(215 - 115 * t) + tom;

            Color cor = { r, g, b, 255 };

            DrawRectangle(k * TAM_TILE, i * TAM_TILE, TAM_TILE, TAM_TILE, cor);
        }
    }
}

void desenhar_chao(jogo *j) {
    Rectangle src = {
        0, 0,
        (float)j->tex_areia.width,
        (float)j->tex_areia.height
    };

    Rectangle dst = {
        0,
        (float)CHAO,
        (float)LARGURA,
        (float)ALT_CHAO
    };

    Vector2 origem = { 0, 0 };

    DrawTexturePro(j->tex_areia, src, dst, origem, 0.0f, WHITE);
}


void texto_sombra(const char *t, int x, int y, int tam, Color cor) {
    Color sombra = { 0, 20, 50, 160 };

    DrawText(t, x + 2, y + 2, tam, sombra);
    DrawText(t, x, y, tam, cor);
}

void texto_centro(const char *t, int y, int tam, Color cor) {
    texto_sombra(t, (LARGURA - MeasureText(t, tam)) / 2, y, tam, cor);
}

void desenhar_hud(jogo *j) {
    texto_sombra(TextFormat("Pontos: %d", j->pontos), 20, 15, 30, RAYWHITE);
    texto_sombra(TextFormat("Nivel: %d", 1 + j->pontos / 5), 20, 50, 20, SKYBLUE);
}

void desenhar_game_over(jogo *j) {
    Color escuro = { 0, 10, 40, 140 };

    DrawRectangle(0, 0, LARGURA, ALTURA, escuro);

    texto_centro("GAME OVER", 70, 56, RED);
    texto_centro(TextFormat("Pontos: %d", j->pontos), 140, 30, RAYWHITE);
    texto_centro("Pressione R para reiniciar", 200, 22, RAYWHITE);
}

void iniciar_jogo(jogo *j) {
    j->algas = NULL;

    j->tex_alga = LoadTexture("assets/alga.png");
    j->tex_areia = LoadTexture("assets/areia.png");

    SetTextureFilter(j->tex_alga, TEXTURE_FILTER_BILINEAR);

    iniciar_fundo(j);
}

void fim_de_jogo(jogo *j) {
    liberar_algas(j);

    UnloadTexture(j->tex_alga);
    UnloadTexture(j->tex_areia);
}

void resetar_jogo(jogo *j, jogador *p) {
    liberar_algas(j);

    j->pontos = 0;
    j->velocidade = VEL_INICIAL;
    j->abertura = ABERTURA_INICIAL;

    resetar_jogador(p);
    atualizar_algas(j);
}

int colidir(jogo *j, Rectangle hit) {
    alga *a;

    for (a = j->algas; a != NULL; a = a->proximo) {

        float hx = a->x + (LARG_ALGA - LARG_HITBOX_ALGA) / 2.0f;
        float fim_cima = a->abertura_y - MARGEM_ALGA;
        float ini_baixo = a->abertura_y + a->abertura + MARGEM_ALGA;

        Rectangle cima = {
            hx, 0.0f,
            LARG_HITBOX_ALGA,
            fim_cima
        };

        Rectangle baixo = {
            hx,
            ini_baixo,
            LARG_HITBOX_ALGA,
            CHAO - ini_baixo
        };

        if (CheckCollisionRecs(hit, cima) || CheckCollisionRecs(hit, baixo)) {
            return 1;
        }
    }

    return 0;
}

int atualizar_jogo(jogo *j, jogador *p, int pulou) {
    alga *a;

    atualizar_jogador(p, pulou);
    atualizar_algas(j);

    for (a = j->algas; a != NULL; a = a->proximo) {
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

    if (hit.y < 0 || hit.y + hit.height > CHAO) {
        return 1;
    }

    return colidir(j, hit);
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

            desenhar_fundo(&partida);
            desenhar_algas(&partida);
            desenhar_chao(&partida);
            desenhar_jogador(&polvo);
            desenhar_hud(&partida);

            if (morreu) {
                desenhar_game_over(&partida);
            }

        EndDrawing();
    }

    fim_de_jogo(&partida);
    UnloadTexture(polvo.textura);

    CloseWindow();

    return 0;
}
