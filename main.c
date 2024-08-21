#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "func.h"


float SPEED = 5;
int state = 0;



StackNode* high_scores = NULL;

bool key[4] = {false, false, false, false}; // [0] = up, [1] = down, [2] = left, [3] = right
bool redraw = true;
bool doexit = false;
int draw_shade=0;
bool animation=0;

Car car;
Obstacle obstacle;
int score = 0;
int score_draw=0;
bool high_id=0;

void init_car() {
    car.x = (SCREEN_W / 2) - (CAR_W / 2);
    car.y = SCREEN_H - CAR_H - 10;
}

void init_obstacle() {
    obstacle.x = 70 + rand() % (SCREEN_W - OBSTACLE_W - 140);
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
    al_install_mouse();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);

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

    background_image1 = al_load_bitmap("background1.png");
    if(!background_image1) {
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
        al_destroy_bitmap(background_image1);
        al_destroy_display(display);
        return -1;
    }

    button_play = al_load_bitmap("button_play.png");
    if(!button_play){
        al_destroy_bitmap(car_image);
        al_destroy_bitmap(obstacle_image);
        al_destroy_bitmap(background_image1);
        al_destroy_font(font);
        al_destroy_display(display);
        return -1;
    }
    button_high = al_load_bitmap("button_high.png");
    if(!button_high){
        al_destroy_bitmap(button_play);
        al_destroy_bitmap(car_image);
        al_destroy_bitmap(obstacle_image);
        al_destroy_bitmap(background_image1);
        al_destroy_font(font);
        al_destroy_display(display);
        return -1;
    }
    button_exit = al_load_bitmap("button_exit.png");
    if(!button_exit){
        al_destroy_bitmap(button_play);
        al_destroy_bitmap(button_high);
        al_destroy_bitmap(car_image);
        al_destroy_bitmap(obstacle_image);
        al_destroy_bitmap(background_image1);
        al_destroy_font(font);
        al_destroy_display(display);
        return -1;
    }

    shade = al_load_bitmap("shade.png");
    if(!shade){
        al_destroy_bitmap(button_play);
        al_destroy_bitmap(button_high);
        al_destroy_bitmap(button_exit);
        al_destroy_bitmap(car_image);
        al_destroy_bitmap(obstacle_image);
        al_destroy_bitmap(background_image1);
        al_destroy_font(font);
        al_destroy_display(display);
        return -1;
    }

    background_image2 = al_load_bitmap("background2.png");
    if(!background_image2){
        al_destroy_bitmap(shade);
        al_destroy_bitmap(button_play);
        al_destroy_bitmap(button_high);
        al_destroy_bitmap(button_exit);
        al_destroy_bitmap(car_image);
        al_destroy_bitmap(obstacle_image);
        al_destroy_bitmap(background_image1);
        al_destroy_font(font);
        al_destroy_display(display);
        return -1;
    }

    trophy = al_load_bitmap("trophy.png");
    if(!trophy){
        al_destroy_bitmap(background_image2);
        al_destroy_bitmap(shade);
        al_destroy_bitmap(button_play);
        al_destroy_bitmap(button_high);
        al_destroy_bitmap(button_exit);
        al_destroy_bitmap(car_image);
        al_destroy_bitmap(obstacle_image);
        al_destroy_bitmap(background_image1);
        al_destroy_font(font);
        al_destroy_display(display);
        return -1;
    }



    timer = al_create_timer(1.0 / 144);
    event_queue = al_create_event_queue();

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());


    al_start_timer(timer);

    init_car();
    init_obstacle();
    load_scores(&high_scores);
    ordering_scores(high_scores);

    while(!doexit) {
        
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);
        if(state==0){
        if(ev.type == ALLEGRO_EVENT_TIMER){
            redraw = 1;
            for(int i=0;i<4;i++){
                key[i]= false;
            }
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            doexit=true;
        }else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            int mouse_x = ev.mouse.x;
            int mouse_y = ev.mouse.y;
            printf("x: %d , y: %dz\n", mouse_x,mouse_y);
            if (mouse_x > 144 && mouse_x < 307 && mouse_y > 246 && mouse_y < 270) {
                    state = 1;
            }
            if (mouse_x > 143 && mouse_x < 306 && mouse_y > 299 && mouse_y < 317) {
                    state = 2;
            }
            if (mouse_x > 193 && mouse_x < 254 && mouse_y > 347 && mouse_y < 367) {
                    state = 3;
            }
        }else if(ev.type == ALLEGRO_EVENT_MOUSE_AXES){
            if(ev.mouse.x > 144 && ev.mouse.x < 307 && ev.mouse.y > 246 && ev.mouse.y <270){
                draw_shade=1;
            }else if(ev.mouse.x > 143 && ev.mouse.x < 306 && ev.mouse.y > 299 && ev.mouse.y <317){
                draw_shade=2;
            }else if(ev.mouse.x > 193 && ev.mouse.x < 254 && ev.mouse.y > 347 && ev.mouse.y <367){
                draw_shade=3;
            }else
                draw_shade =0;
        }
        if(redraw && al_is_event_queue_empty(event_queue)){
            redraw = 0;
            al_clear_to_color(al_map_rgb(36, 56, 85));
            if(draw_shade == 1){
                al_draw_bitmap(shade,(SCREEN_W/2)- (SHADE_W/2), SCREEN_H/3 - 25, ALLEGRO_ALIGN_CENTRE);
            }
            if(draw_shade == 2){
                al_draw_bitmap(shade,(SCREEN_W/2)- (SHADE_W/2), SCREEN_H/3 + 25, ALLEGRO_ALIGN_CENTRE);
            }
            if(draw_shade == 3){
                al_draw_bitmap(shade,(SCREEN_W/2)- (SHADE_W/2), SCREEN_H/3 + 70, ALLEGRO_ALIGN_CENTRE);
            }
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 150, ALLEGRO_ALIGN_CENTER, "-Car On TOP-");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 250, ALLEGRO_ALIGN_CENTER, "Start Game");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 300, ALLEGRO_ALIGN_CENTER, "High Scores");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 350, ALLEGRO_ALIGN_CENTER, "Exit");
            al_flip_display();
        }
        }else if(state==1){
        if(ev.type == ALLEGRO_EVENT_TIMER) {
            if(SPEED < 10){
                SPEED *= 1.0001;
            } 
            if(score >= 100 && SPEED < 13){
                SPEED *= 1.00001;
            }
            if(key[0] && car.y > 0) {
                car.y -= SPEED*0.5;
            }
            if(key[1] && car.y < SCREEN_H - CAR_H) {
                car.y += SPEED*0.5;
            }
            if(key[2] && car.x > 70) {
                car.x -= SPEED*0.5;
            }
            if(key[3] && car.x < SCREEN_W - CAR_W - 70) {
                car.x += SPEED*0.5;
            }

            update_obstacle();

            if (check_collision(&car, &obstacle)) {
                if(high_scores->score < score){
                    high_id = true;
                }
                push_score(&high_scores, score);
                ordering_scores(high_scores);
                save_scores(high_scores);
                score_draw = score;
                score = 0;
                init_car();
                init_obstacle();
                SPEED = 5;
                state = 4;
            }

            redraw = true;
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            doexit=true;
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
            if(animation == 0){
                al_draw_bitmap(background_image1, 0, 0, 0);
                animation =1;
            }else if(animation == 1){
                al_draw_bitmap(background_image2, 0, 0, 0);
                animation=0;
            }
            

            // Desenha o carro e o obstáculo
            al_draw_bitmap(car_image, car.x, car.y, 0);
            al_draw_bitmap(obstacle_image, obstacle.x, obstacle.y, 0);

            // Desenha a pontuação e os high scores
            al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0, "Score: %d", score);
            al_draw_textf(font, al_map_rgb(255, 255, 255), 150, 10, 0, "SPEED: %.2f Km/h", SPEED*10);

            StackNode* current = high_scores;
            ordering_scores(current);
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 30, 0, "High Score:");
            al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 50, 0, "%d", current->score);

            al_flip_display();
        }
        }else if(state == 2){
            if(ev.type == ALLEGRO_EVENT_TIMER){
                redraw = 1;
                
            }else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                doexit = true;
            }else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            int mouse_x = ev.mouse.x;
            int mouse_y = ev.mouse.y;
            printf("x: %d , y: %dz\n", mouse_x,mouse_y);
            if(mouse_x > 358 && mouse_x < 438 && mouse_y > 755 && mouse_y <784){
                state = 0;
            }
            if(mouse_x > 9 && mouse_x < 82 && mouse_y > 755 && mouse_y <784){
                state = 3;
            }
            
            }else if(ev.type == ALLEGRO_EVENT_MOUSE_AXES){
            if(ev.mouse.x > 358 && ev.mouse.x < 438 && ev.mouse.y > 755 && ev.mouse.y <784){
                draw_shade=1; 
            }else if(ev.mouse.x > 9 && ev.mouse.x < 82 && ev.mouse.y > 755 && ev.mouse.y <784){
                draw_shade=2; 
            }else
                draw_shade=0;
            }
            
            if(redraw && al_is_event_queue_empty(event_queue)){
                redraw =0;

                al_clear_to_color(al_map_rgb(36, 56, 85));
                if(draw_shade == 1){
                al_draw_bitmap(shade,350, 750, ALLEGRO_ALIGN_LEFT);
                }
                if(draw_shade == 2){
                al_draw_bitmap(shade,-365, 750, ALLEGRO_ALIGN_CENTER);
                }
                StackNode* current = high_scores;
                ordering_scores(current);
                int y = 200;
                int cont = 1;
                while (current != NULL) {
                al_draw_textf(font, al_map_rgb(255, 255, 255), SCREEN_W/2, y, ALLEGRO_ALIGN_CENTRE, "%d. %d",cont, current->score);
                current = current->next;
                cont++;
                y += 30;
                }
                al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 150, ALLEGRO_ALIGN_CENTER, "-HIGH SOCORES-");
                al_draw_text(font, al_map_rgb(255, 255, 255), 20 , 760, ALLEGRO_ALIGN_LEFT, "Exit");
                al_draw_text(font, al_map_rgb(255, 255, 255), 430, 760, ALLEGRO_ALIGN_RIGHT, "Back");
                al_draw_bitmap(trophy, SCREEN_W/2 +40, 190, ALLEGRO_ALIGN_CENTER);
                al_flip_display();
            }

        }else if(state == 3){
            doexit = true;
        }else if(state == 4){
            if(ev.type == ALLEGRO_EVENT_TIMER){
                redraw = 1;
            }else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                doexit = true;
            }else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                int mouse_x = ev.mouse.x;
                int mouse_y = ev.mouse.y;
                printf("x: %d , y: %dz\n", mouse_x,mouse_y);
                if(mouse_x > 358 && mouse_x < 438 && mouse_y > 755 && mouse_y <784){
                state = 0;
            }
            }else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES){
                if(ev.mouse.x > 358 && ev.mouse.x < 438 && ev.mouse.y > 755 && ev.mouse.y <784){
                draw_shade=1; 
                }
            }
            if(redraw && al_is_event_queue_empty(event_queue)){
                redraw = 0;
                printf("DESENHOUUUUU!!!");
                al_clear_to_color(al_map_rgb(36, 56, 85));
                al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 150, ALLEGRO_ALIGN_CENTER, "-GAME OVER-");
                al_draw_text(font, al_map_rgb(255, 255, 255), 20 , 760, ALLEGRO_ALIGN_LEFT, "Play again");
                al_draw_text(font, al_map_rgb(255, 255, 255), 430, 760, ALLEGRO_ALIGN_RIGHT, "Main menu");
                if(high_id == false){

                }
            }
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
    al_destroy_bitmap(background_image1);
    al_destroy_bitmap(background_image2);
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}