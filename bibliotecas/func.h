#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#define SCREEN_W 450
#define SCREEN_H 800
#define CAR_W 50
#define CAR_H 100
#define OBSTACLE_W 50
#define OBSTACLE_H 100
#define PLAY_H 198
#define PLAY_W 47
#define HIGH_H 198
#define HIGH_W 45
#define EXIT_H 198
#define EXIT_W 46

static ALLEGRO_FONT *font = NULL;
static ALLEGRO_BITMAP *button_play = NULL;
static ALLEGRO_BITMAP *button_high = NULL;
static ALLEGRO_BITMAP *button_exit = NULL;

typedef struct {
    float x, y;
} Car;

typedef struct {
    float x, y;
} Obstacle;

typedef struct StackNode {
    int score;
    struct StackNode* next;
} StackNode;


void push_score(StackNode** stack, int score);
void save_scores(StackNode* stack);
void load_scores(StackNode** stack);

void init_car();
