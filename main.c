#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "func.h"



//Inicializando a lista do high score
StackNode* high_scores = NULL;

//Inicializando algumas variaveis de controle
bool key[4] = {false, false, false, false}; // [0] = up, [1] = down, [2] = left, [3] = right
bool redraw = true;
bool doexit = false;
int draw_shade=0;
bool animation=0;
float SPEED = 5;
int state = 0;
int score = 0;
int score_draw=0;
bool high_id=0;

//Inicializando os as posições do carro e do obstaculo
Car car;
Obstacle obstacle;
//Inicia o carro e o obstaculo

void init_car() {
    car.x = (SCREEN_W / 2) - (CAR_W / 2);
    car.y = SCREEN_H - CAR_H - 10;
}

void init_obstacle() {
    obstacle.x = 70 + rand() % (SCREEN_W - OBSTACLE_W - 140);
    obstacle.y = -OBSTACLE_H;
}
//"Cria" um novo obstaculo e soma os pontos
void update_obstacle() {
    obstacle.y += SPEED;
    if (obstacle.y > SCREEN_H) {
        score++;
        init_obstacle();
    }
}
// verifica a colisão do carro com o obstaculo
bool check_collision(Car* car, Obstacle* obstacle) {
    return !(car->x > obstacle->x + OBSTACLE_W || 
             car->x + CAR_W < obstacle->x || 
             car->y > obstacle->y + OBSTACLE_H || 
             car->y + CAR_H < obstacle->y);
}

int main(int argc, char **argv) {
    srand(time(NULL));
    //Inicia o Allegro5
    al_init();
    if(!al_init()) {
        fprintf(stderr, "Falha ao inicializar Allegro!\n");
        return -1;
    }
    //Instala os recursos necessarios
    al_init_image_addon();
    al_init_primitives_addon();
    al_install_mouse();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_audio();
    al_reserve_samples(1);
    al_init_acodec_addon();
    al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);

    //Carregando os recursos principais
    display = al_create_display(SCREEN_W, SCREEN_H);
    if(!display) {
        printf("Falha ao criar display!\n");
        return -1;
    }

    car_image = al_load_bitmap("assets/car.png");
    if(!car_image) {
        printf("Falha ao carregar a imagem do carro!\n");
        doexit = true;
    }

    obstacle_image = al_load_bitmap("assets/obstacle.png");
    if(!obstacle_image) {
        printf("Falha ao carregar a imagem do obstáculo!\n");
        doexit = true;
    }

    background_image1 = al_load_bitmap("assets/background1.png");
    background_image2 = al_load_bitmap("assets/background2.png");
    if(!background_image1||!background_image2) {
        printf("Falha ao carregar a imagem de fundo!\n");
        doexit = true;
    }

    font = al_load_font("assets/font.TTF", 18, 0);
    if(!font) {
        printf("Falha ao carregar a fonte!\n");
        doexit = true;
    }
    //Carregando os recursos extras
    button_play = al_load_bitmap("assets/button_play.png");
    button_high = al_load_bitmap("assets/button_high.png");
    button_exit = al_load_bitmap("assets/button_exit.png");
    shade = al_load_bitmap("assets/shade.png");
    trophy_gold = al_load_bitmap("assets/trophy_gold.png");
    trophy_silver = al_load_bitmap("assets/trophy_silver.png");
    trophy_bronze = al_load_bitmap("assets/trophy_bronze.png");
    over = al_load_bitmap("assets/over.png");
    new_h = al_load_bitmap("assets/new_h.png");
    ferrari = al_load_bitmap("assets/ferrari.png");
    if(!button_play||!button_high||!button_exit||!shade||!trophy_gold||!trophy_silver||!trophy_bronze||!over||!new_h||!ferrari){
        printf("Falha ao carregar alguma imagem extra!\n");
        doexit = true;
    }
    //Carregando a musica de fundo
    tokyo = al_load_sample("assets/tokyo.mp3");
    if(!tokyo){
        printf("Falha ao carregar a musica");
        doexit = true;
    }

    //Inicializando linha de eventos base e oque deve registrar
    timer = al_create_timer(1.0 / 144);
    event_queue = al_create_event_queue();

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());


    al_start_timer(timer);

    //Inicia carro, obstaculo e a lista de high score
    init_car();
    init_obstacle();
    load_scores(&high_scores);
    ordering_scores(high_scores);
    
    //Começa o ciclo do jogo
    while(!doexit) {
        //Declara um uma lista de eventos "ev".
        ALLEGRO_EVENT ev;
        //Espera por alguma ação
        al_wait_for_event(event_queue, &ev);
        //Inicia a musica em loop
        al_play_sample(tokyo, 0.1,0,1,ALLEGRO_PLAYMODE_LOOP,NULL);
        //Jogo se baseia em estados, quando em 0 = menu,  1 = jogo, 2 = menu de highscores, 3 = exit, 4 = janela de game over
        if(state==0){
        //Menu completo
        if(ev.type == ALLEGRO_EVENT_TIMER){
            redraw = 1;
            for(int i=0;i<4;i++){
                key[i]= false;
            }
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            doexit=true;
            //Verifica os click nos botões
        }else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            int mouse_x = ev.mouse.x;
            int mouse_y = ev.mouse.y;
            if (mouse_x > 144 && mouse_x < 307 && mouse_y > 246 && mouse_y < 270) {
                    state = 1;
            }
            if (mouse_x > 143 && mouse_x < 306 && mouse_y > 299 && mouse_y < 317) {
                    state = 2;
            }
            if (mouse_x > 193 && mouse_x < 254 && mouse_y > 347 && mouse_y < 367) {
                    state = 3;
            }
        // Adiciona "sombra" ao passar o mouse encima
        }else if(ev.type == ALLEGRO_EVENT_MOUSE_AXES){
            if(ev.mouse.x > 144 && ev.mouse.x < 307 && ev.mouse.y > 246 && ev.mouse.y <270){
                draw_shade=1;
            }else if(ev.mouse.x > 143 && ev.mouse.x < 306 && ev.mouse.y > 299 && ev.mouse.y <317){
                draw_shade=2;
            }else if(ev.mouse.x > 193 && ev.mouse.x < 254 && ev.mouse.y > 347 && ev.mouse.y <367){
                draw_shade=3;
            }else
                draw_shade =0;
        } //Desenha o menu e as "sombras"
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
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 150, ALLEGRO_ALIGN_CENTER, "-Driving On TOP-");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 250, ALLEGRO_ALIGN_CENTER, "Start Game");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 300, ALLEGRO_ALIGN_CENTER, "High Scores");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 350, ALLEGRO_ALIGN_CENTER, "Exit");
            al_draw_bitmap(ferrari, SCREEN_W/2-100, SCREEN_H/2, ALLEGRO_ALIGN_CENTRE);
            al_draw_text(font, al_map_rgb(183,184,168), 20, SCREEN_H/2+40, ALLEGRO_ALIGN_LEFT, "Integrantes:");
            al_draw_text(font, al_map_rgb(183,184,168), 20, SCREEN_H/2+80, ALLEGRO_ALIGN_LEFT, "Eduardo Almeida,");
            al_draw_text(font, al_map_rgb(183,184,168), 20, SCREEN_H/2+110, ALLEGRO_ALIGN_LEFT, "Samuel Detone,");
            al_draw_text(font, al_map_rgb(183,184,168), 20, SCREEN_H/2+140, ALLEGRO_ALIGN_LEFT, "Vitto Baroni");
            al_flip_display();
        }
        }else if(state==1){
            //Jogabilidade completa
        if(ev.type == ALLEGRO_EVENT_TIMER) {
            if(high_scores != NULL){
                //Verifica se o score atual é o highscore
            if(score > high_scores->score){
                high_id = 1;
            }else{
                high_id = 0;
            }
            }else{
                high_id =1;
            }
            //Aumenta a velocidade do jogo e do carro
            if(SPEED < 10){
                SPEED *= 1.0001;
            } 
            if(score >= 100 && SPEED < 13){
                SPEED *= 1.00001;
            }
            // Move o carro em relação a tecla apertada
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
            //Atualiza o obstaculo acrescentando os pontos
            update_obstacle();
            //Verifica a colisão = game over
            if (check_collision(&car, &obstacle)) {
                push_score(&high_scores, score);
                ordering_scores(high_scores);
                save_scores(high_scores);
                score_draw = score;
                score = 0;
                init_car();
                init_obstacle();
                SPEED = 5;
                state = 4;
                redraw=1;
            }

            redraw = true;
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            doexit=true;
        }//Identifica qual tecla esta precionada e qual não
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
        }// Desenha a tela de jogo
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

            // Desenha a pontuação, o highscore e a velocidade do carro
            al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0, "Score: %d", score);
            al_draw_textf(font, al_map_rgb(255, 255, 255), 150, 10, 0, "SPEED: %.2f Km/h", SPEED*10);

            StackNode* current = high_scores;
            ordering_scores(current);
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 30, 0, "High Score:");
            if(current != NULL){
            al_draw_textf(font, al_map_rgb(255, 255, 255), SCREEN_W/2-50, 30, 0, "%d", current->score);
            }
            al_flip_display();
        }
        }else if(state == 2){
            // Menu de highscore completo
            if(ev.type == ALLEGRO_EVENT_TIMER){
                redraw = 1;
                
            }else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                doexit = true;
                // Verifica os click nos botões
            }else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            int mouse_x = ev.mouse.x;
            int mouse_y = ev.mouse.y;
            if(mouse_x > 358 && mouse_x < 438 && mouse_y > 755 && mouse_y <784){
                state = 0;
            }
            if(mouse_x > 9 && mouse_x < 82 && mouse_y > 755 && mouse_y <784){
                state = 3;
            }
            // Adiciona "sombra" ao passar o mouse encima
            }else if(ev.type == ALLEGRO_EVENT_MOUSE_AXES){
            if(ev.mouse.x > 358 && ev.mouse.x < 438 && ev.mouse.y > 755 && ev.mouse.y <784){
                draw_shade=1; 
            }else if(ev.mouse.x > 9 && ev.mouse.x < 82 && ev.mouse.y > 755 && ev.mouse.y <784){
                draw_shade=2; 
            }else
                draw_shade=0;
            }
             // Desenha a janela
            if(redraw && al_is_event_queue_empty(event_queue)){
                redraw =0;

                al_clear_to_color(al_map_rgb(36, 56, 85));
                //Desenha as "sombras"
                if(draw_shade == 1){
                al_draw_bitmap(shade,350, 750, ALLEGRO_ALIGN_LEFT);
                }
                if(draw_shade == 2){
                al_draw_bitmap(shade,-365, 750, ALLEGRO_ALIGN_CENTER);
                }
                //Desenha os botões e os highscores
                StackNode* current = high_scores;
                ordering_scores(current);
                int y = 200;
                int cont = 1;
                while (current != NULL) {
                al_draw_textf(font, al_map_rgb(255, 255, 255), SCREEN_W/2 -30, y, ALLEGRO_ALIGN_LEFT, "%d. %d",cont, current->score);
                current = current->next;
                cont++;
                y += 30;
                }
                al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 150, ALLEGRO_ALIGN_CENTER, "-HIGH SOCORES-");
                al_draw_text(font, al_map_rgb(255, 255, 255), 20 , 760, ALLEGRO_ALIGN_LEFT, "Exit");
                al_draw_text(font, al_map_rgb(255, 255, 255), 430, 760, ALLEGRO_ALIGN_RIGHT, "Back");
                //Desenha algumas imagens para complementar
                al_draw_bitmap(trophy_gold, SCREEN_W/2 -60, 198, ALLEGRO_ALIGN_CENTER);
                al_draw_bitmap(trophy_silver, SCREEN_W/2 -60, 228, ALLEGRO_ALIGN_CENTER);
                al_draw_bitmap(trophy_bronze, SCREEN_W/2 -60, 258, ALLEGRO_ALIGN_CENTER);
                al_flip_display();
            }
            // O state = 3 conclui o loop do jogo e o finaliza
        }else if(state == 3){
            doexit = true;
        }else if(state == 4){
            //Janela de game over completa
           if(ev.type == ALLEGRO_EVENT_TIMER){
            redraw = 1;
            for(int i=0;i<4;i++){
                key[i]= false;
            }
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            doexit=true;

            //Verifica os click nos botões
        }else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            int mouse_x = ev.mouse.x;
            int mouse_y = ev.mouse.y;
            if (mouse_x > 246 && mouse_x < 391 && mouse_y > 299 && mouse_y < 321) {
                    state = 0;
            }
            if (mouse_x > 53 && mouse_x < 208 && mouse_y > 299 && mouse_y < 321) {
                    state = 1;
            }
            // Adiciona "sombra" ao passar o mouse encima
        }else if(ev.type == ALLEGRO_EVENT_MOUSE_AXES){
            if(ev.mouse.x > 246 && ev.mouse.x < 391 && ev.mouse.y > 299 && ev.mouse.y <321){
                draw_shade=1;
            }else if(ev.mouse.x > 53 && ev.mouse.x < 208 && ev.mouse.y > 299 && ev.mouse.y <321){
                draw_shade=2;
            }else
                draw_shade =0;
        }//Desenha as informações
        if(redraw && al_is_event_queue_empty(event_queue)){
            redraw = 0;
            al_clear_to_color(al_map_rgb(36, 56, 85));
            //Desenha a "sombra"
            if(draw_shade == 1){
                al_draw_bitmap(shade,230, 290, ALLEGRO_ALIGN_LEFT);
            }
            if(draw_shade == 2){
                al_draw_bitmap(shade,-230, 290, ALLEGRO_ALIGN_CENTER);
            }
            //Desenha algumas informações e os botões
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 150, ALLEGRO_ALIGN_CENTER, "-GAME OVER-");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2 -20 ,SCREEN_H/2- 100, ALLEGRO_ALIGN_RIGHT, "Play again");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2 + 30, SCREEN_H/2-100, ALLEGRO_ALIGN_LEFT, "Main menu");
            //Desenha 2 game overs diferentes com base no score
            if(high_id == false){
                al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2 -15, 200, ALLEGRO_ALIGN_CENTER, "Score:");
                al_draw_textf(font, al_map_rgb(255, 255, 255), SCREEN_W/2 + 45, 200, ALLEGRO_ALIGN_LEFT, "%d",score_draw);
                al_draw_bitmap(over, SCREEN_W/2 -165, SCREEN_H/2, ALLEGRO_ALIGN_CENTER);
            }else if(high_id == true){
                al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2 -10, 200, ALLEGRO_ALIGN_CENTER, "New High Score:");
                al_draw_textf(font, al_map_rgb(255, 255, 255), SCREEN_W/2 + 110, 200, ALLEGRO_ALIGN_LEFT, "%d",score_draw);
                al_draw_bitmap(new_h, SCREEN_W/2 -200, SCREEN_H/2, ALLEGRO_ALIGN_CENTER);
            }
            al_flip_display();
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
    //Destroi todos os objetos para finalizar o jogo
    al_destroy_bitmap(car_image);
    al_destroy_bitmap(obstacle_image);
    al_destroy_bitmap(background_image1);
    al_destroy_bitmap(background_image2);
    al_destroy_bitmap(button_play);
    al_destroy_bitmap(button_high);
    al_destroy_bitmap(button_exit);
    al_destroy_bitmap(shade);
    al_destroy_bitmap(trophy_gold);
    al_destroy_bitmap(trophy_silver);
    al_destroy_bitmap(trophy_bronze);
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_sample(tokyo);

    return 0;
}