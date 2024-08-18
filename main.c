#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_W 450
#define SCREEN_H 800
#define CAR_W 50
#define CAR_H 100
#define OBSTACLE_W 50
#define OBSTACLE_H 100
float SPEED = 5;


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

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_BITMAP *car_image = NULL;
ALLEGRO_BITMAP *obstacle_image = NULL;
ALLEGRO_BITMAP *background_image = NULL;
ALLEGRO_FONT *font = NULL;

bool key[4] = {false, false, false, false}; // [0] = up, [1] = down, [2] = left, [3] = right
bool redraw = true;
bool doexit = false;

Car car;
Obstacle obstacle;
int score = 0;

StackNode* high_scores = NULL;

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

void init_car() {
    car.x = (SCREEN_W / 2) - (CAR_W / 2);
    car.y = SCREEN_H - CAR_H - 10;
}

void init_obstacle() {
    obstacle.x = rand() % (SCREEN_W - OBSTACLE_W);
    obstacle.y = -OBSTACLE_H;
}

void update_obstacle() {
    obstacle.y += SPEED;
    if (obstacle.y > SCREEN_H) {
        score++;
        init_obstacle();
    }
}

bool check_collision(Car* car, Obstacle* obstacle) {
    return !(car->x > obstacle->x + OBSTACLE_W || 
             car->x + CAR_W < obstacle->x || 
             car->y > obstacle->y + OBSTACLE_H || 
             car->y + CAR_H < obstacle->y);
}

int main(int argc, char **argv) {
    srand(time(NULL));

    if(!al_init()) {
        fprintf(stderr, "Falha ao inicializar Allegro!\n");
        return -1;
    }

    al_init_image_addon();
    al_init_primitives_addon();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();

    display = al_create_display(SCREEN_W, SCREEN_H);
    if(!display) {
        fprintf(stderr, "Falha ao criar display!\n");
        return -1;
    }

    car_image = al_load_bitmap("car.png");
    if(!car_image) {
        fprintf(stderr, "Falha ao carregar a imagem do carro!\n");
        al_destroy_display(display);
        return -1;
    }

    obstacle_image = al_load_bitmap("obstacle.png");
    if(!obstacle_image) {
        fprintf(stderr, "Falha ao carregar a imagem do obstáculo!\n");
        al_destroy_bitmap(car_image);
        al_destroy_display(display);
        return -1;
    }

    background_image = al_load_bitmap("background.png");
    if(!background_image) {
        fprintf(stderr, "Falha ao carregar a imagem de fundo!\n");
        al_destroy_bitmap(car_image);
        al_destroy_bitmap(obstacle_image);
        al_destroy_display(display);
        return -1;
    }

    font = al_load_font("font.TTF", 18, 0);
    if(!font) {
        fprintf(stderr, "Falha ao carregar a fonte!\n");
        al_destroy_bitmap(car_image);
        al_destroy_bitmap(obstacle_image);
        al_destroy_bitmap(background_image);
        al_destroy_display(display);
        return -1;
    }

    timer = al_create_timer(1.0 / 144);
    event_queue = al_create_event_queue();

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    al_start_timer(timer);

    init_car();
    init_obstacle();
    load_scores(&high_scores);

    while(!doexit) {
        if(SPEED < 10){
            SPEED *= 1.0001;
        } 
        if(score >= 100 && SPEED < 13){
            SPEED *= 1.00001;
        }
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_TIMER) {
            if(key[0] && car.y > 0) {
                car.y -= SPEED*0.5;
            }
            if(key[1] && car.y < SCREEN_H - CAR_H) {
                car.y += SPEED*0.5;
            }
            if(key[2] && car.x > 0) {
                car.x -= SPEED*0.5;
            }
            if(key[3] && car.x < SCREEN_W - CAR_W) {
                car.x += SPEED*0.5;
            }

            update_obstacle();

            if (check_collision(&car, &obstacle)) {
                push_score(&high_scores, score);
                save_scores(high_scores);
                score = 0;
                init_car();
                init_obstacle();
                SPEED = 5;
            }

            redraw = true;
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            doexit = true;
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch(ev.keyboard.keycode) {
                case ALLEGRO_KEY_W:
                    key[0] = true;
                    break;
                case ALLEGRO_KEY_UP:
                    key[0] = true;
                    break;
                case ALLEGRO_KEY_S:
                    key[1] = true;
                    break;
                case ALLEGRO_KEY_DOWN:
                    key[1] = true;
                    break;
                case ALLEGRO_KEY_A:
                    key[2] = true;
                    break;
                case ALLEGRO_KEY_LEFT:
                    key[2] = true;
                    break;
                case ALLEGRO_KEY_D:
                    key[3] = true;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    key[3] = true;
                    break;
            }
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
            switch(ev.keyboard.keycode) {
                case ALLEGRO_KEY_W:
                    key[0] = false;
                    break;
                case ALLEGRO_KEY_UP:
                    key[0] = false;
                    break;
                case ALLEGRO_KEY_S:
                    key[1] = false;
                    break;
                case ALLEGRO_KEY_DOWN:
                    key[1] = false;
                    break;
                case ALLEGRO_KEY_A:
                    key[2] = false;
                    break;
                case ALLEGRO_KEY_LEFT:
                    key[2] = false;
                    break;
                case ALLEGRO_KEY_D:
                    key[3] = false;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    key[3] = false;
                    break;
            }
        }

        if(redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;

            // Desenha a imagem de fundo
            al_draw_bitmap(background_image, 0, 0, 0);

            // Desenha o carro e o obstáculo
            al_draw_bitmap(car_image, car.x, car.y, 0);
            al_draw_bitmap(obstacle_image, obstacle.x, obstacle.y, 0);

            // Desenha a pontuação e os high scores
            al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0, "Score: %d", score);
            al_draw_textf(font, al_map_rgb(255, 255, 255), 150, 10, 0, "SPEED: %.2f Km/h", SPEED*10);

            StackNode* current = high_scores;
            int y = 50;
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 30, 0, "High Scores:");
            while (current != NULL) {
                al_draw_textf(font, al_map_rgb(255, 255, 255), 10, y, 0, "%d", current->score);
                current = current->next;
                y += 20;
            }

            al_flip_display();
        }
    }

    // Limpeza de memória
    StackNode* current = high_scores;
    while (current != NULL) {
        StackNode* temp = current;
        current = current->next;
        free(temp);
    }

    al_destroy_bitmap(car_image);
    al_destroy_bitmap(obstacle_image);
    al_destroy_bitmap(background_image);
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}