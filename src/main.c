#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "kigyopalya.h"
#include <stdio.h>
#include <SDL_mixer.h>
#include "debugmalloc.h"
#ifdef _WIN32
    #include <windows.h>
#endif

/* beállítható, de legyenek 80 többszörösei, és a kigyopalya.h-ban a pálya méreteit is meg kell változtatni! */
const int ablakszel = 720;
const int ablakmag = 720;

/* külön függvénybe, hogy olvashatóbb legyen */
void sdl_init(char const *felirat, int szeles, int magas, SDL_Window **pwindow, SDL_Renderer **prenderer) {

    /* mindent inicializálunk */
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }

    /* létrehozunk egy ablakot */
    SDL_Window *window = SDL_CreateWindow(felirat, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szeles, magas, 0);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }

    /* a memóriában az interfész a program és a grafika között, itt tárolódnak a megjelenítendő elemek */
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }

    /* kitisztítja a renderert az alapszínnel (ez állítható) */
    SDL_RenderClear(renderer);

    /* az ablak és renderer pointereket cím szerint módosítjuk, átállítjuk a fenti függvények által visszaadott, inicializált struktúrákra */
    *pwindow = window;
    *prenderer = renderer;
}

Uint32 idozit(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;   /* ujabb varakozas */
}

int main(int argc, char *argv[]) {
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif
    srand(time(NULL));
    /* ablak létrehozása */
    int szelesseg = ablakszel;
    int magassag = ablakmag;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Rect src = { 0, 0, 720, 720 };
    SDL_Rect dest = {0, 0, 720, 720 };
    /* SDL inicializálás, ablakfejléc */
    sdl_init("Boldog születésnapot Attila !!!", szelesseg, magassag, &window, &renderer);
    SDL_Texture *drill = (SDL_Texture*)IMG_LoadTexture(renderer, "drill.png");
    SDL_RenderCopy(renderer, drill, &src, &dest);
    SDL_RenderPresent(renderer);

    SDL_TimerID hatter = SDL_AddTimer(2000, idozit, NULL);
    SDL_Event jatekstart;
    do{SDL_WaitEvent(&jatekstart);}while(jatekstart.type != SDL_USEREVENT);
    SDL_RemoveTimer(hatter);

    /*char zenek[10][10];
    for(int i=0;i<10;++i){
        sprintf(zenek[i],"zene%d.mp3",i);
        printf("%s\n",zenek[i]);
    }*/
    int zeneszam = rand()%10;
    char aktualis_zene[10];
    sprintf(aktualis_zene,"zene%d.mp3",zeneszam);
    SDL_TimerID id = SDL_AddTimer(250, idozit, NULL);
    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Music* zene = Mix_LoadMUS(aktualis_zene);
    Mix_PlayMusic(zene,-1);
    /* kígyó létrehozás, inicializálás */
    Kigyo attila;
    kigyo_init(&attila);
    SDL_Texture *background = (SDL_Texture*)IMG_LoadTexture(renderer, "background.png");

    /* közvetlen azelőtt deklaráljuk, hogy használjuk */
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        SDL_WaitEvent(&event);

        SDL_RenderCopy(renderer, background, &src, &dest);
        char pontok[20];
        sprintf(pontok, "Pontok: %d", attila.hossz);
        stringRGBA(renderer, 320, 10, pontok , 255, 255, 255, 255);
        bool rajzoltam = false;
        switch (event.type) {
            case SDL_USEREVENT: kigyo_cb(&attila,utemezett); rajzoltam = true; break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_UP: kigyo_cb(&attila,le_megnyom); rajzoltam = true; break;
                    case SDLK_DOWN: kigyo_cb(&attila,fel_megnyom);rajzoltam= true; break;
                    case SDLK_LEFT: kigyo_cb(&attila,bal_megnyom); rajzoltam = true; break;
                    case SDLK_RIGHT: kigyo_cb(&attila,jobb_megnyom); rajzoltam = true; break;
                    case SDLK_ESCAPE: kigyo_cb(&attila,escape_megnyom); break;
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_UP: kigyo_cb(&attila,le_elenged); rajzoltam = true; break;
                    case SDLK_DOWN: kigyo_cb(&attila,fel_elenged); rajzoltam= true; break;
                    case SDLK_LEFT: kigyo_cb(&attila,bal_elenged); rajzoltam = true; break;
                    case SDLK_RIGHT: kigyo_cb(&attila,jobb_elenged); rajzoltam = true; break;
                    case SDLK_ESCAPE: kigyo_cb(&attila,escape_elenged); quit = true; break;

                }
                break;



            /* ablak bezarasa */
            case SDL_QUIT:
                SDL_RemoveTimer(id);
                quit = true;
                break;
        }
        /////////////////////////////switch event type
        if (rajzoltam){
            //printf("(x=%d\ty=%d)\n",attila.feje->hely.x, attila.feje->hely.y);
            //if(attila.meghalt)printf("meghalt\n");
            //else printf("nem halt meg\n");
            kigyo_rajzol(&attila, renderer);
            SDL_RenderPresent(renderer);
        }
        if(attila.meghalt){
            SDL_RemoveTimer(id);
            Mix_Chunk *hang3 = Mix_LoadWAV("hang3.wav");
            Mix_PlayChannel(-1,hang3,0);

            break;
        }


    }
    filledCircleRGBA(renderer, 360, 360, 150, 0, 0, 0, 255);
    char veguzenet1[30];
    char veguzenet2[20];
    char veguzenet3[30];
    sprintf(veguzenet1, "Sajnos vege a jateknak!");
    sprintf(veguzenet2, "Pontszamod: %d",attila.hossz);
    sprintf(veguzenet3, "Nyomj escape-t a kilepeshez!");
    stringRGBA(renderer, 270, 310, veguzenet1 , 255, 255, 255, 255);
    stringRGBA(renderer, 300, 340, veguzenet2 , 255, 255, 255, 255);
    stringRGBA(renderer, 260, 370, veguzenet3 , 255, 255, 255, 255);
    SDL_RenderPresent(renderer);
    SDL_Event event;
    while(SDL_WaitEvent(&event)){
    if(event.type == SDL_KEYUP){
        if(event.key.keysym.sym == SDLK_ESCAPE){
            SDL_Quit();
            break;
        }
    }}

    /* ablak bezarasa */
    SDL_Quit();
    kigyo_felszabadit(&attila);
    Mix_FreeMusic(zene);
    return 0;
}
