
#ifndef RECORDES_H
#define RECORDES_H

#define MAX_RECORDES      5
#define ARQUIVO_RECORDES  "recordes.txt"

void carregar_recordes(int recordes[]);          
void salvar_recordes(int recordes[]);            
int entra_no_ranking(int recordes[], int pontos); 
void inserir_recorde(int recordes[], int pontos);

#endif
