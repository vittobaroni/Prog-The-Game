#include <stdio.h>
#include <stdlib.h>
#include "func.h"

void push_score(StackNode** stack, int score) {
    StackNode* new_node = (StackNode*) malloc(sizeof(StackNode));
    new_node->score = score;
    new_node->next = *stack;
    *stack = new_node;
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