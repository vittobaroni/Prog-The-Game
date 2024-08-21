#include <stdio.h>
#include <stdlib.h>
#include "func.h"


void search_score(StackNode* stack, int score){
    StackNode* aux = malloc(sizeof(StackNode));
    aux = stack;
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

void push_score(StackNode** stack, int score) {
    StackNode* new_node = malloc(sizeof(StackNode));
    if(cont >= MAX_STACK){
        search_score(*stack,score);
    }else{
    new_node->score = score;
    new_node->next = *stack;
    new_node->prev = NULL;
    *stack = new_node;
    cont++;
    }
}

void ordering_scores(StackNode* stack){
    StackNode* aux = malloc(sizeof(StackNode));
    aux = stack;
    while(aux->next != NULL){
        StackNode* aux2 = malloc(sizeof(StackNode));
        aux2 = aux;
        while (aux2->next != NULL)
        {
            if(aux2->score < aux2->next->score){
                int tmp = aux2->score;
                printf("%d ",tmp);
                aux2->score = aux2->next->score;
                aux2->next->score = tmp;
            }
            aux2 = aux2->next;
        }
        aux = aux->next;
    }
    
}

void save_scores(StackNode* stack) {
    FILE *file = fopen("high_scores.txt", "w");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo para escrita.\n");
        return;
    }
    StackNode* current = stack;
    while (current != NULL) {
        fprintf(file, "%d\n", current->score);
        current = current->next;
    }
    fclose(file);
}

void load_scores(StackNode** stack) {
    FILE *file = fopen("high_scores.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo para leitura.\n");
        return;
    }
    int score;
    while (fscanf(file, "%d", &score) != EOF) {
        push_score(stack, score);
    }
    fclose(file);
}