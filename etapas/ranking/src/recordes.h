#ifndef RECORDES_H
#define RECORDES_H

#define MAX_RECORDES     5
#define ARQUIVO_RECORDES "recordes.txt"

/**
 * @brief Carrega os recordes salvos do arquivo para o array.
 * @param recordes Array onde os recordes serão armazenados.
 */
void carregar_recordes(int recordes[]);

/**
 * @brief Salva o array atual de recordes no arquivo.
 * @param recordes Array contendo os recordes atuais.
 */
void salvar_recordes(const int recordes[]);

/**
 * @brief Verifica se uma pontuação é alta o suficiente para entrar no ranking.
 * @param recordes Array atual de recordes.
 * @param pontos Pontuação a ser testada.
 * @return int Retorna 1 se entra no ranking, 0 caso contrário.
 */
int entra_no_ranking(const int recordes[], int pontos);

/**
 * @brief Insere uma nova pontuação na posição correta do ranking.
 * @param recordes Array atual de recordes.
 * @param pontos Pontuação a ser inserida.
 */
void inserir_recorde(int recordes[], int pontos);

#endif // RECORDES_H
