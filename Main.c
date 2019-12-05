#define SDL_MAIN_HANDLED
#include <stdio.h>
#include "SDL2/include/SDL.h"
#include "SDL2/include/SDL_timer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <string.h>

typedef struct Brick //Brick structures which holds 4 elements
{
  //Elements are used for x and y position of the brick and its width and height
  float x;
  float y;
  float width;
  float height;
  int alive; //Alive variable. We use to to store in which state the brick is (broken, not broken)
}brick_t;

typedef struct player 
{
  float x;
  float y;
  float width;
  float height;
  float speed;
}Player_t;

typedef struct ball 
{
  float x;
  float y;
  float width;
  float height;
  float speed_x;
  float speed_y;
}ball_t;

int checkCollision(float Ax, float Ay, float Aw, float Ah, float Bx, float By, float Bw, float Bh) //Funcfion for checking collision
{
  if ( Ay+Ah < By ) return 0; //if A is more to the left than B
  else if ( Ay > By+Bh ) return 0; //if A is more to the right than B
  else if ( Ax+Aw < Bx ) return 0; //if A is higher than B
  else if ( Ax > Bx+Bw ) return 0; //if A is lower than B
 
  return 1; //There is a collision because none of above returned false
}

int die(char* message)
{
    SDL_Log("%s : %s",message,SDL_GetError());
    SDL_Quit();
    return 0;
}

int end()
{
    SDL_Quit();
    return 0;
}

int main(int argc ,char **argv)
{
    //inizializzo le sezioni della libreria sdl che mi interessano , in questo caso solo la parte video
    if(SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("error initializing SDL2: %s", SDL_GetError());
    }

    //Creo la finestra dove mostrare il contenuto delle cose
    SDL_Window* window = SDL_CreateWindow("Game", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    
    if(!window)
    {
        return die("error creating window: %s");
    }

    //Creo il renderer che disegnera il necessario sulla finesra creata in precedenza
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    
    if(!renderer)
    {
        return die("unable to create renderer");
    }

    Player_t player;
    player.x = 260;
    player.y = 400;
    player.width = 80;
    player.height = 20;
    player.speed =20;
    
    ball_t ball;
    ball.x = 300;
    ball.y = 370;
    ball.width =15;
    ball.height =15;
    ball.speed_x =5;
    ball.speed_y = -5;

    int BRICKS = 45;
    brick_t bricks[45]; 
    for ( int n = 0, x = 4, y = 10; n < BRICKS; n++, x+=70 ) //A for loop that goes throught the array so we can set the positions
    {
      if ( x > 600 ) //If x is near the right edge of the screen
        {
          x = 4; //We start going from the left again
          y += 25; //And move a down a little
        }
      bricks[n].x = x; //Set currents bricks x position
      bricks[n].y = y; //Y position
      bricks[n].width = 60; //Width
      bricks[n].height = 20; //Height
      bricks[n].alive = 1; //Set alive to true (not broken)
    }
    
    int close_requested = 0;
    int up = 0;
    int down = 0;
    int left = 0;
    int right = 0;
    for(;;){//loop infinito
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                close_requested = 1;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    up = 1;
                    break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    player.x-=player.speed;
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    down = 1;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    player.x+=player.speed;
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    up = 0;
                    break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    left = 0;
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    down = 0;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    right = 0;
                    break;
                }
                break;
            }
        }
         ball.x += ball.speed_x; //Move the ball on x axis first
 
      for ( int n = 0; n < BRICKS; n++ ) //Go throught the array of bricks
        {
          if ( bricks[n].alive == 1 ) //If the bricks is alive
            {
              if ( checkCollision(ball.x,ball.y,ball.width,ball.height,bricks[n].x,bricks[n].y,bricks[n].width, bricks[n].height)) //Check for collision with the ball
                {
                  ball.speed_x = -ball.speed_x; //Change x velocity of the ball
                  bricks[n].alive = 0; //Set the alive variable to false (brick is broken)
                  break; //Stop checking for collision on x axis
                }
            }
        }
 
      ball.y += ball.speed_y; //move the ball on y axis second
 
      for ( int n = 0; n < BRICKS; n++ ) //Go throught the array of bricks
        {
          if ( bricks[n].alive == 1 ) //If the brick is alive
            {
              if ( checkCollision(ball.x,ball.y,ball.width,ball.height,bricks[n].x,bricks[n].y,bricks[n].width, bricks[n].height)) //Check for collision
                {
                  ball.speed_y = -ball.speed_y; //Change y velocity of the ball
                  bricks[n].alive = 0; //Set alive varible to false
                  break; //Stop checking for collision on y axis
                }
            }
        }
 
      if ( ball.x < 0 ) //Check if the ball hit the left edge of screen
        {
          ball.speed_x = -ball.speed_x; //negate the x velocity
        }
 
      else if ( ball.x+ball.height >600 )
        {
          ball.speed_x = -ball.speed_x;
        }
 
      if ( ball.y < 0 )
        {
          ball.speed_y = -ball.speed_y;
        }
 
      else if ( ball.y+ball.height > 400 ) //if the ball hit the bottom edge of screen
        {
            //lose
        }

        //pulisco lo schermo con il nero
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
       
        
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_TRANSPARENT); 
        SDL_Rect rect ;
        rect.x = player.x; // the rectangle
        rect.y = player.y;
        rect.w = player.width;
        rect.h = player.height;
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0,0, 255, SDL_ALPHA_TRANSPARENT); 
        
        rect.x = ball.x; // the rectangle
        rect.y = ball.y;
        rect.w = ball.width;
        rect.h = ball.height;
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_TRANSPARENT); 
        for (int i = 0; i < BRICKS; i++)
        {
            if (bricks[i].alive)
            {
                rect.x = bricks[i].x; // the rectangle
                rect.y = bricks[i].y;
                rect.w = bricks[i].width;
                rect.h = bricks[i].height;
            }
                SDL_RenderFillRect(renderer, &rect);
                
        }
        //SDL_RenderDrawLine(renderer, start_x, start_y, start_x + offeset, start_y + offeset);
        SDL_RenderPresent(renderer);
    }   

    SDL_Quit();
    return 0;
}