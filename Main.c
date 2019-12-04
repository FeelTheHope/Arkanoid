#define SDL_MAIN_HANDLED
#include <stdio.h>
#include "SDL2/include/SDL.h"
#include "SDL2/include/SDL_timer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <string.h>

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

    //setto la posizione da dove deve partire a diseganre il renderer ed il suo offset
    int start_x = 100;
    int start_y = 100;
    int offeset = 200;

    //Creo una texture vuota , specificando le sue caratteristiche come il colore o come deve essere salvata e grandezza
    SDL_Texture* texture=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,640,400);

    //creo un puntatore ad una locazione di memoria dove verranno allocate le informazioni sui pixel
    Uint8 *pixels = NULL;
    int pitch = 0;

    //alloco una porzione di memoria nell heap per copiare l immagine
    if (SDL_LockTexture(texture,NULL,(void **)&pixels,&pitch))
    {
        return die("unable to map texture into address space");
    }

    SDL_Log("texture to mapped at %p with pitch %d",pixels,pitch);
    //int optimaizingpitch = pitch / 4;
    //put_pixel((Uint32 **)pixels,optimaizingpitch,50,50,0xFFFF00FF);

    // creo i valori necessari per la foto , come altezza e larghezza 
    int width,height,comp;

    // creo un puntatore di memoria dove verra salvata la texture dove indico il nome dell'immagine, altezza, larghezza 
    unsigned char *image = stbi_load("resources/hello.png",&width,&height,&comp,STBI_rgb_alpha);
    if(!image)
    {
        return die("unable to load image");
    }
    memcpy(pixels,image,pitch*180);
    free(image); //libero lo spazio occupato per copiare l immagine
    SDL_UnlockTexture(texture);

    int up,down,left,right = 0;
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
                    left = 1;
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    down = 1;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    right = 1;
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

        //pulisco lo schermo con il nero
        //SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        //disegno una linea rossa
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_TRANSPARENT);
        //SDL_RenderDrawLine(renderer, start_x, start_y, start_x + offeset, start_y + offeset);
        SDL_Rect rect;
        rect.x=10;
        rect.y=10;
        rect.w=256;
        rect.h=256;

        SDL_RenderCopy(renderer,texture,NULL,&rect);
        //screen update
        SDL_RenderPresent(renderer);
    }   

    SDL_Quit();
    return 0;
}