#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

// Define tamanhos de alguns objetos e o tamanho maximo da lista
#define SCREEN_W 450
#define SCREEN_H 800
#define CAR_W 50
#define CAR_H 100
#define OBSTACLE_W 50
#define OBSTACLE_H 100
#define PLAY_W 198
#define PLAY_H 47
#define HIGH_W 198
#define HIGH_H 45
#define EXIT_W 198
#define EXIT_H 46
#define SHADE_W 450
#define SHADE_H 30
#define TROPHY_W 40
#define TROPHY_H 40
#define MAX_STACK 10

// Declarando recursos usados no jogo
static ALLEGRO_FONT *font = NULL;
static ALLEGRO_BITMAP *button_play = NULL;
static ALLEGRO_BITMAP *button_high = NULL;
static ALLEGRO_BITMAP *button_exit = NULL;
static ALLEGRO_BITMAP *shade = NULL;
static ALLEGRO_BITMAP *over = NULL;
static ALLEGRO_BITMAP *new_h = NULL;
static ALLEGRO_DISPLAY *display = NULL;
static ALLEGRO_EVENT_QUEUE *event_queue = NULL;
static ALLEGRO_TIMER *timer = NULL;
static ALLEGRO_BITMAP *car_image = NULL;
static ALLEGRO_BITMAP *obstacle_image = NULL;
static ALLEGRO_BITMAP *background_image1 = NULL;
static ALLEGRO_BITMAP *background_image2 = NULL;
static ALLEGRO_BITMAP *ferrari = NULL;
static ALLEGRO_BITMAP *trophy_gold = NULL;
static ALLEGRO_BITMAP *trophy_silver = NULL;
static ALLEGRO_BITMAP *trophy_bronze = NULL;
static ALLEGRO_SAMPLE* tokyo = NULL;



//Algumas structs
typedef struct {
    float x, y;
} Car;

typedef struct {
    float x, y;
} Obstacle;

typedef struct StackNode {
    int score;
    struct StackNode* next;
    struct StackNode* prev;
} StackNode;
//Contador usando na ordenação de scores
static int cont=0;
// Declara as funções do score.c
void push_score(StackNode** stack, int score);
void save_scores(StackNode* stack);
void load_scores(StackNode** stack);
void ordering_scores(StackNode* stack);