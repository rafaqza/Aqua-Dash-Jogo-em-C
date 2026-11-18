
#include "raylib.h"
#include <stdlib.h>
#include "recordes.h"

#define LARGURA 800
#define ALTURA 600
#define ALT_CHAO 60
#define CHAO (ALTURA - ALT_CHAO)

#define LARG_ALGA 140.0f
#define ALT_ALGA 400.0f
#define LARG_HITBOX_ALGA 50.0f
#define MARGEM_ALGA 15.0f
#define ESPACO_ALGAS 300.0f

#define LARG_OURO 48.0f   
#define ALT_OURO 42.0f
#define VALOR_OURO 5
#define DESVIO_OURO 175   

#define VEL_INICIAL 3.0f
#define VEL_MAX 7.5f
#define ABERTURA_INICIAL 210.0f
#define ABERTURA_MIN 140.0f

#define TAM_TILE 20
#define LINHAS_FUNDO 30
#define COLUNAS_FUNDO 40

#define ESTADO_MENU 0
#define ESTADO_JOGANDO 1
#define ESTADO_FIM 2

typedef struct {
    Vector2 posicao;
    float velocidade_y;
    float gravidade;
    float pulo;
    float largura;
    float altura;
    Texture2D textura;
    int personagem;    /* 0 = polvo, 1 = segundo personagem */
} jogador;

typedef struct alga {
    float x;
    float abertura_y;
    float abertura;
    int pontuada;
    float ouro_y;
    int ouro_ativo;
    struct alga *proximo;
} alga;

typedef struct {
    alga *algas;
    int fundo[LINHAS_FUNDO][COLUNAS_FUNDO];
    Texture2D tex_alga;
    Texture2D tex_areia;
    Texture2D tex_ouro;
    int pontos;
    int ouros;
    float velocidade;
    float abertura;
} jogo;

void resetar_jogador(jogador *p) {
    p->posicao.x = 150.0f;
    p->posicao.y = CHAO / 2.0f;
    p->velocidade_y = 0.0f;
}

/* troca o personagem e ajusta a largura (cada imagem tem uma proporcao) */
void escolher_personagem(jogador *p, int numero) {
    p->personagem = numero;

    if (numero == 0) {
        p->largura = p->altura * p->textura.width / p->textura.height;
    } else {
        p->largura = p->altura;   /* por enquanto e so um quadrado branco */
    }
}

void trocar_personagem(jogador *p) {
    if (p->personagem == 0) {
        escolher_personagem(p, 1);
    } else {
        escolher_personagem(p, 0);
    }
}

void iniciar_jogador(jogador *p) {
    p->textura = LoadTexture("assets/polvo_pirata.png");
    SetTextureFilter(p->textura, TEXTURE_FILTER_BILINEAR);

    p->altura = 72.0f;
    escolher_personagem(p, 0);
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

    if (p->personagem == 0) {
        DrawTexturePro(p->textura, src, dst, origem, rot, WHITE);
    } else {
        DrawRectanglePro(dst, origem, rot, WHITE);
    }
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

    /* ouro no meio do caminho entre a alga anterior e esta */
    if (ultima == NULL) {
        nova->ouro_y = 0.0f;
        nova->ouro_ativo = 0;
    } else {
        float centro_anterior = ultima->abertura_y + ultima->abertura / 2.0f;
        float centro_novo = y + j->abertura / 2.0f;

        nova->ouro_y = (centro_anterior + centro_novo) / 2.0f
                       + (float)GetRandomValue(-DESVIO_OURO, DESVIO_OURO);

        if (nova->ouro_y < 60.0f) nova->ouro_y = 60.0f;
        if (nova->ouro_y > CHAO - 60.0f) nova->ouro_y = CHAO - 60.0f;

        nova->ouro_ativo = 1;
    }

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

/* ---------- ouro ---------- */

/* o ouro fica no meio entre esta alga e a anterior (elas sempre ficam
   ESPACO_ALGAS de distancia), entao nao precisa guardar o x dele */
float ouro_x(alga *a) {
    return a->x + (LARG_ALGA - ESPACO_ALGAS) / 2.0f;
}

/* area do bau na tela (serve pra desenhar e pra coletar) */
Rectangle retangulo_ouro(alga *a) {
    Rectangle r = {
        ouro_x(a) - LARG_OURO / 2.0f,
        a->ouro_y - ALT_OURO / 2.0f,
        LARG_OURO,
        ALT_OURO
    };

    return r;
}

/* cada ouro pego vale VALOR_OURO pontos */
int pontuacao_total(jogo *j) {
    return j->pontos + j->ouros * VALOR_OURO;
}

/* percorre a lista e pega os ouros que o polvo encostou */
void coletar_ouros(jogo *j, Rectangle corpo) {
    alga *a;

    for (a = j->algas; a != NULL; a = a->proximo) {
        if (a->ouro_ativo && CheckCollisionRecs(corpo, retangulo_ouro(a))) {
            a->ouro_ativo = 0;
            j->ouros++;
        }
    }
}

void desenhar_ouros(jogo *j) {
    Rectangle src = {
        0, 0,
        (float)j->tex_ouro.width,
        (float)j->tex_ouro.height
    };

    Vector2 origem = { 0, 0 };

    alga *a;

    for (a = j->algas; a != NULL; a = a->proximo) {
        if (a->ouro_ativo) {
            Rectangle dst = retangulo_ouro(a);

            DrawTexturePro(j->tex_ouro, src, dst, origem, 0.0f, WHITE);
        }
    }
}

/* ---------- fundo (matriz) e chao ---------- */

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

void desenhar_ranking(int recordes[], int y) {
    int i;

    texto_centro("TOP SCORES", y, 24, GOLD);

    for (i = 0; i < MAX_RECORDES; i++) {
        texto_centro(TextFormat("%d.  %d pontos", i + 1, recordes[i]),
                     y + 36 + i * 26, 20, RAYWHITE);
    }
}

void desenhar_hud(jogo *j) {
    texto_sombra(TextFormat("Pontos: %d", pontuacao_total(j)), 20, 15, 30, RAYWHITE);
    texto_sombra(TextFormat("Nivel: %d", 1 + j->pontos / 5), 20, 50, 20, SKYBLUE);
    texto_sombra(TextFormat("Ouro: %d", j->ouros), 20, 75, 20, GOLD);
}

void desenhar_menu(int recordes[], int personagem) {
    texto_centro("AquaDash", 70, 60, GOLD);
    texto_centro("ESPACO para nadar", 140, 22, RAYWHITE);
    texto_centro("Pressione ESPACO para comecar", 170, 22, RAYWHITE);
    texto_centro("Colete os baus de ouro: +5 pontos cada", 200, 20, GOLD);
    texto_centro(TextFormat("TAB = trocar personagem (atual: %d)", personagem + 1), 232, 20, SKYBLUE);

    desenhar_ranking(recordes, 300);
}

void desenhar_fim_de_jogo(jogo *j, int recordes[], int novo_recorde) {
    Color escuro = { 0, 10, 40, 140 };

    DrawRectangle(0, 0, LARGURA, ALTURA, escuro);

    texto_centro("GAME OVER", 70, 56, RED);
    texto_centro(TextFormat("Pontos: %d", pontuacao_total(j)), 140, 30, RAYWHITE);
    texto_centro(TextFormat("Ouro coletado: %d", j->ouros), 178, 20, GOLD);

    if (novo_recorde) {
        texto_centro("NOVO RECORDE!", 208, 28, GOLD);
    }

    desenhar_ranking(recordes, 260);
    texto_centro("Pressione R para reiniciar", 480, 22, RAYWHITE);
}

void iniciar_jogo(jogo *j) {
    j->algas = NULL;

    j->tex_alga = LoadTexture("assets/alga.png");
    j->tex_areia = LoadTexture("assets/areia.png");
    j->tex_ouro = LoadTexture("assets/bau.png");

    SetTextureFilter(j->tex_alga, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(j->tex_ouro, TEXTURE_FILTER_BILINEAR);

    iniciar_fundo(j);
}

void fim_de_jogo(jogo *j) {
    liberar_algas(j);

    UnloadTexture(j->tex_alga);
    UnloadTexture(j->tex_areia);
    UnloadTexture(j->tex_ouro);
}

void resetar_jogo(jogo *j, jogador *p) {
    liberar_algas(j);

    j->pontos = 0;
    j->ouros = 0;
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

    /* coleta de ouro usa o corpo todo do polvo (mais facil de pegar) */
    Rectangle corpo = { p->posicao.x, p->posicao.y, p->largura, p->altura };
    coletar_ouros(j, corpo);

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

    int recordes[MAX_RECORDES];

    iniciar_jogador(&polvo);
    iniciar_jogo(&partida);

    carregar_recordes(recordes);
    resetar_jogo(&partida, &polvo);

    int estado = ESTADO_MENU;
    int novo_recorde = 0;

    float flutuar = 0.0f;
    float flutuar_vel = 0.5f;

    while (!WindowShouldClose()) {
        int pulou = IsKeyPressed(KEY_SPACE) ||
                    IsKeyPressed(KEY_UP) ||
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (estado == ESTADO_MENU) {
            flutuar += flutuar_vel;

            if (flutuar > 10.0f || flutuar < -10.0f) {
                flutuar_vel = -flutuar_vel;
            }

            polvo.posicao.y = CHAO / 2.0f + flutuar;

            if (IsKeyPressed(KEY_TAB)) {
                trocar_personagem(&polvo);
            }

            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                resetar_jogo(&partida, &polvo);
                estado = ESTADO_JOGANDO;
            }

        } else if (estado == ESTADO_JOGANDO) {

            if (atualizar_jogo(&partida, &polvo, pulou)) {
                estado = ESTADO_FIM;

                novo_recorde = entra_no_ranking(recordes, pontuacao_total(&partida));

                if (novo_recorde) {
                    inserir_recorde(recordes, pontuacao_total(&partida));
                    salvar_recordes(recordes);
                }
            }

        } else {

            if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_ENTER)) {
                resetar_jogo(&partida, &polvo);
                estado = ESTADO_JOGANDO;
            }
        }

        BeginDrawing();

            desenhar_fundo(&partida);

            if (estado != ESTADO_MENU) {
                desenhar_algas(&partida);
                desenhar_ouros(&partida);
            }

            desenhar_chao(&partida);
            desenhar_jogador(&polvo);

            if (estado == ESTADO_MENU) {
                desenhar_menu(recordes, polvo.personagem);
            } else {
                desenhar_hud(&partida);

                if (estado == ESTADO_FIM) {
                    desenhar_fim_de_jogo(&partida, recordes, novo_recorde);
                }
            }

        EndDrawing();
    }

    fim_de_jogo(&partida);
    UnloadTexture(polvo.textura);

    CloseWindow();

    return 0;
}
