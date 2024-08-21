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
#define PLAY_W 198
#define PLAY_H 47
#define HIGH_W 198
#define HIGH_H 45
#define EXIT_W 198
#define EXIT_H 46
#define SHADE_W 450
#define SHADE_H 30
#define MAX_STACK 10

static ALLEGRO_FONT *font = NULL;
static ALLEGRO_BITMAP *button_play = NULL;
static ALLEGRO_BITMAP *button_high = NULL;
static ALLEGRO_BITMAP *button_exit = NULL;
static ALLEGRO_BITMAP *shade = NULL;
static ALLEGRO_DISPLAY *display = NULL;
static ALLEGRO_EVENT_QUEUE *event_queue = NULL;
static ALLEGRO_TIMER *timer = NULL;
static ALLEGRO_BITMAP *car_image = NULL;
static ALLEGRO_BITMAP *obstacle_image = NULL;
static ALLEGRO_BITMAP *background_image1 = NULL;
static ALLEGRO_BITMAP *background_image2 = NULL;



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

static int cont=0;

void push_score(StackNode** stack, int score);
void save_scores(StackNode* stack);
void load_scores(StackNode** stack);
void ordering_scores(StackNode** stack);