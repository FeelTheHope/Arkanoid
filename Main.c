#define SDL_MAIN_HANDLED
#include <stdio.h>
#include "SDL2/include/SDL.h"
#include "SDL2/include/SDL_timer.h"
#include "SDL2/include/SDL_mixer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <string.h>



Uint64 NOW, LAST;
double deltaTime = 0;


static const char *MY_COOL_MP3 = "high.wav";

    
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


    //audio
    // int result = 0;
    // int flags = MIX_INIT_MP3;
    // if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    //     printf("Failed to init SDL\n");
    //     exit(1);
    // }

    //  if (flags != (result = Mix_Init(flags))) {
    //     printf("Could not initialize mixer (result: %d).\n", result);
    //     printf("Mix_Init: %s\n", Mix_GetError());
    //     exit(1);
    // }

    // Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
    // Mix_Music *music = Mix_LoadMUS(MY_COOL_MP3);
    // Mix_PlayMusic(music, 1);


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

    //Creo una texture vuota , specificando le sue caratteristiche come il colore o come deve essere salvata e grandezza
    SDL_Texture* texture=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGB888,SDL_TEXTUREACCESS_STREAMING,256,256);
    SDL_Texture* texture1=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGB888,SDL_TEXTUREACCESS_STREAMING,256,256);
    SDL_Texture* texture2=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGB888,SDL_TEXTUREACCESS_STREAMING,256,256);
    SDL_Texture* texture3=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGB888,SDL_TEXTUREACCESS_STREAMING,256,256);
    
    
   

    //creo un puntatore ad una locazione di memoria dove verranno allocate le informazioni sui pixel
    Uint8 *pixels = NULL;
    int pitch = 0;

    Uint8 *pixels1 = NULL;
    int pitch1 = 0;

    Uint8 *pixels2 = NULL;
    int pitch2 = 0;

    Uint8 *pixels3 = NULL;
    int pitch3 = 0;
 
 

    //alloco una porzione di memoria nell heap per copiare l immagine
    if (SDL_LockTexture(texture,NULL,(void **)&pixels,&pitch))
    {
        return die("unable to map texture into address space");
    }
    SDL_Log("texture to mapped at %p with pitch %d",pixels,pitch);


     if (SDL_LockTexture(texture1,NULL,(void **)&pixels1,&pitch1))
    {
        return die("unable to map texture into address space");
    }

    SDL_Log("texture to mapped at %p with pitch %d",pixels1,pitch1);


     if (SDL_LockTexture(texture2,NULL,(void **)&pixels2,&pitch2))
    {
        return die("unable to map texture into address space");
    }

    SDL_Log("texture to mapped at %p with pitch %d",pixels3,pitch3);

     if (SDL_LockTexture(texture3,NULL,(void **)&pixels3,&pitch3))
    {
        return die("unable to map texture into address space");
    }

    SDL_Log("texture to mapped at %p with pitch %d",pixels3,pitch3);
    

   

     
    //int optimaizingpitch = pitch / 4;
    //put_pixel((Uint32 **)pixels,optimaizingpitch,50,50,0xFFFF00FF);

    // creo i valori necessari per la foto , come altezza e larghezza 
    int width,height,comp;

    // creo un puntatore di memoria dove verra salvata la texture dove indico il nome dell'immagine, altezza, larghezza 
    unsigned char *image = stbi_load("Images/GameOver.png",&width,&height,&comp,STBI_rgb_alpha);
    unsigned char *p1 = stbi_load("Images/p1.png",&width,&height,&comp,STBI_rgb_alpha);
    unsigned char *back = stbi_load("Images/back.png",&width,&height,&comp,STBI_rgb_alpha);
    unsigned char *palla = stbi_load("Images/ball.png",&width,&height,&comp,STBI_rgb_alpha);



    if(!palla)
    {
      return die("unable to load image");
    }
          
    memcpy(pixels3,palla,pitch3*256);
    free(palla); //libero lo spazio occupato per copiare l immagine
    SDL_UnlockTexture(texture3);


    if(!image)
    {
      return die("unable to load image");
    }
          
    memcpy(pixels2,image,pitch2*256);
    free(image); //libero lo spazio occupato per copiare l immagine
    SDL_UnlockTexture(texture2);

   

    if(!back)
    {
      return die("unable to load image");
    }

    memcpy(pixels1,back,pitch1*256);
    free(back); //libero lo spazio occupato per copiare l immagine
    SDL_UnlockTexture(texture1);


     if(!p1)
    {
      return die("unable to load image");
    }
          
    memcpy(pixels,p1,pitch*256);
    free(p1); //libero lo spazio occupato per copiare l immagine
    SDL_UnlockTexture(texture);


    Player_t player;
    player.x = 260;
    player.y = 400;
    player.width = 80;
    player.height = 20;
    player.speed =2.00;

          

    ball_t ball;
    ball.x = 300;
    ball.y = 370;
    ball.width =15;
    ball.height =15;
    ball.speed_x =2;
    ball.speed_y = -2;

    int BRICKS = 48;
    int remaining_bricks = 45;
    brick_t bricks[48];
    for ( int n = 0, x = 45, y = 20; n < BRICKS; n++, x+=70 ) //A for loop that goes throught the array so we can set the positions
    {
      if ( x > 550 ) //If x is near the right edge of the screen
        {
          x = 45; //We start going from the left again
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

     LAST = NOW;
     NOW = SDL_GetPerformanceCounter();
     deltaTime = (double)((NOW - LAST)*1000 / (double)SDL_GetPerformanceFrequency() );

     const Uint8 *state = SDL_GetKeyboardState(NULL);
     if (state[SDL_SCANCODE_LEFT])
    {
        
      player.x-=player.speed;
                   
    }
    if (state[SDL_SCANCODE_RIGHT])
    {
      player.x+=player.speed ;
    }
    
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
               
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    down = 1;
                    break;
                case SDL_SCANCODE_D:
               
                    
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
                  bricks[n].alive = 0;
                  remaining_bricks =remaining_bricks -1 ;

                  if (remaining_bricks % 10 == 0)
                  {
                    ball.speed_x = ball.speed_x * 1.2f;
                    ball.speed_y = ball.speed_y * 1.2f;
                  }
                  
                  if (remaining_bricks % 20 == 0)
                  {
                    player.speed = player.speed * 1.5f;
                  }
                  
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
                  remaining_bricks =remaining_bricks -1 ;
                  if (remaining_bricks % 10 == 0)
                  {
                    ball.speed_x = ball.speed_x * 1.2f;
                    ball.speed_y = ball.speed_y * 1.2f;
                  }
                  
                  if (remaining_bricks % 20 == 0)
                  {
                    player.speed = player.speed * 1.5f;
                  }
                  break; //Stop checking for collision on y axis
                }

               
            }
        }

        if ( checkCollision(ball.x,ball.y,ball.width,ball.height,player.x,player.y,player.width, player.height)) //Check for collision with the ball
                {
                  ball.speed_y = -ball.speed_y; //Change x velocity of the ball
                }

      if ( ball.x < 0 ) //Check if the ball hit the left edge of screen
        {
          ball.speed_x = -ball.speed_x;
        }

      else if ( ball.x+ball.height >600 )
        {
          ball.speed_x = -ball.speed_x;
        }

      if ( ball.y < 0 )
        {
          ball.speed_y = -ball.speed_y;
        }

      else if ( ball.y+ball.height > 500 ) //if the ball hit the bottom edge of screen
        {
          SDL_Rect rect ; 
          rect.x = 0; // the rectangle
          rect.y = 0;
          rect.w = 640;
          rect.h = 480;

          

          SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
          SDL_RenderClear(renderer);
          SDL_RenderCopy(renderer,texture2,NULL,&rect);
          SDL_RenderPresent(renderer);
          ball.x = 300;
          ball.y = 370;
          ball.speed_x = 3.0;
          ball.speed_y = 3.0;

          player.x = 260;
          player.y = 400;
          player.speed = 3.0f;

          remaining_bricks = BRICKS;
          for (int i = 0; i < BRICKS; i++)
          {
            bricks[i].alive = 1;
          }
          
          SDL_Delay(3000);
        }

        SDL_Rect rect1 ; 
        rect1.x = 0; // the rectangle
        rect1.y = 0;
        rect1.w = 640;
        rect1.h = 480;

        //pulisco lo schermo con il nero
        // SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        // SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_TRANSPARENT);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer,texture1,NULL,&rect1);
        

      

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_TRANSPARENT);
        SDL_Rect rect ;
        rect.x = player.x; // the rectangle
        rect.y = player.y;
        rect.w = player.width;
        rect.h = player.height;
        SDL_RenderCopy(renderer,texture,NULL,&rect);

        SDL_SetRenderDrawColor(renderer, 0 ,255, 0, SDL_ALPHA_TRANSPARENT);

        rect.x = ball.x; // the rectangle
        rect.y = ball.y;
        rect.w = ball.width;
        rect.h = ball.height;
        SDL_RenderCopy(renderer,texture3,NULL,&rect);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_TRANSPARENT);
        
        for (int i = 0; i < BRICKS - 40 ; i++)
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
      SDL_SetRenderDrawColor(renderer, 255, 255, 100, SDL_ALPHA_TRANSPARENT);
       for (int i = 8; i  < BRICKS -32 ; i++)
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
         SDL_SetRenderDrawColor(renderer, 0, 100, 255, SDL_ALPHA_TRANSPARENT);
       for (int i = 16 ; i  < BRICKS -24 ; i++)
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
SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_TRANSPARENT);
        for (int i = 24 ; i  < BRICKS - 16 ; i++)
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

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_TRANSPARENT);
        for (int i = 32 ; i  < BRICKS  ; i++)
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
    
    
    // Mix_FreeMusic(music);

    SDL_Quit();    
    return 0;
}