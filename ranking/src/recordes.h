/* recordes.h - [REQ: ARQUIVO] top scores salvos em recordes.txt */
#ifndef RECORDES_H
#define RECORDES_H

#define MAX_RECORDES      5
#define ARQUIVO_RECORDES  "recordes.txt"

void carregar_recordes(int recordes[]);          /* LE do arquivo      */
void salvar_recordes(int recordes[]);            /* ESCREVE no arquivo */
int entra_no_ranking(int recordes[], int pontos); /* 1 = entra, 0 = nao */
void inserir_recorde(int recordes[], int pontos);

#endif
