#include <stdio.h>
#include <stdlib.h>
#include "func.h"

//Determina qual será o valor a ser trocado em caso de lista cheia
void search_score(StackNode* high_scores, int score){
    StackNode* aux = malloc(sizeof(StackNode));
    aux = high_scores;
    int carry=score;
    int ax=carry;
    if(aux != NULL){
        if(carry >= aux->score){
           carry = aux->score;
           aux->score = ax;
        }
        search_score(aux->next, carry);
    }
    
    
}
// Adiciona um novo score a lista
void push_score(StackNode** high_scores, int score) {
    StackNode* new_node = malloc(sizeof(StackNode));
    if(cont >= MAX_STACK){
        search_score(*high_scores,score);
    }else{
    new_node->score = score;
    new_node->next = *high_scores;
    new_node->prev = NULL;
    *high_scores = new_node;
    cont++;
    }
}
// Ordena a lista
void ordering_scores(StackNode* high_scores){
    if(high_scores == NULL){
        return;
    }
    StackNode* aux = malloc(sizeof(StackNode));
    aux = high_scores;
    while(aux->next != NULL){
        StackNode* aux2 = malloc(sizeof(StackNode));
        aux2 = aux;
        while (aux2->next != NULL)
        {
            if(aux2->score < aux2->next->score){
                int tmp = aux2->score;
                aux2->score = aux2->next->score;
                aux2->next->score = tmp;
            }
            aux2 = aux2->next;
        }
        aux = aux->next;
    }
    
}
//Salva a lista no txt "high_scores.txt"
void save_scores(StackNode* high_scores) {
    FILE *file = fopen("high_scores.txt", "w");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo para escrita.\n");
        return;
    }
    StackNode* current = high_scores;
    while (current != NULL) {
        fprintf(file, "%d\n", current->score);
        current = current->next;
    }
    fclose(file);
}
// Le os valores de "high_scores.txt" e implementa a lista high_socres
void load_scores(StackNode** high_scores) {
    FILE *file = fopen("high_scores.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo para leitura.\n");
        return;
    }
    int score;
    while (fscanf(file, "%d", &score) != EOF) {
        push_score(high_scores, score);
    }
    fclose(file);
}