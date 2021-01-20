#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include "kigyopalya.h"
#include <SDL_mixer.h>
#include "debugmalloc.h"


void kigyo_init(Kigyo* melyik_kigyo){
printf("kigyo_init\n");
    melyik_kigyo->iranyban = false;
    melyik_kigyo->meghalt = false;
    melyik_kigyo->hossz = 3;

/* alapból felfelé indul el a kígyó */
    melyik_kigyo->merre_megy= le;

/* kezdetben a kígyó sehol sincs ott */
    for(int i=0;i<pszelesseg+2;++i){
        for(int j=0;j<pmagassag+2;++j){
            (melyik_kigyo->hol_van)[i][j]=semmi;
        }
    }

/* a hol_van tömbbe a kígyó kezdeti megjelenését manuálisan berakjuk */
    melyik_kigyo->hol_van[(pszelesseg+2)/2][(pmagassag+2)/2 + 3] = kigyo;
    melyik_kigyo->hol_van[(pszelesseg+2)/2][(pmagassag+2)/2 + 4] = kigyo;
    melyik_kigyo->hol_van[(pszelesseg+2)/2][(pmagassag+2)/2 + 2] = kigyo;

    melyik_kigyo->merre_nez[(pszelesseg+2)/2][(pmagassag+2)/2 + 3] = le;
    melyik_kigyo->merre_nez[(pszelesseg+2)/2][(pmagassag+2)/2 + 4] = le;
    melyik_kigyo->merre_nez[(pszelesseg+2)/2][(pmagassag+2)/2 + 2] = le;


/* feje, közepe, farka lefoglalása */
    melyik_kigyo->feje = (Kigyodarab*) malloc(sizeof(Kigyodarab));
    melyik_kigyo->farka = (Kigyodarab*) malloc(sizeof(Kigyodarab));
    Kigyodarab* kozepe = (Kigyodarab*) malloc(sizeof(Kigyodarab));

/* pointerek beállítása láncolt listához */
    melyik_kigyo->feje->elozo = NULL;
    melyik_kigyo->feje->kov = kozepe;
    kozepe->elozo = melyik_kigyo->feje;
    kozepe->kov = melyik_kigyo->farka;
    melyik_kigyo->farka->elozo = kozepe;
    melyik_kigyo->farka->kov = NULL;

/* kígyódarabok koordinátáinak beállítása */
    Koord temp1 = {(pszelesseg+2)/2, (pmagassag+2)/2 + 3};
    kozepe->hely = temp1;
    Koord temp2 = {(pszelesseg+2)/2, (pmagassag+2)/2 + 2};
    melyik_kigyo->feje->hely = temp2;
    Koord temp3 = {(pszelesseg+2)/2, (pmagassag+2)/2 + 4};
    melyik_kigyo->farka->hely = temp3;

    kigyo_uj_kaja(melyik_kigyo,zoldseg);
    kigyo_uj_kaja(melyik_kigyo,hus);
}


void kigyo_halad(Kigyo* melyik_kigyo, Irany merre){
printf("kigyo_halad\n");



/* lefoglaljuk az új fejet, amit a kígyó elejére fogunk rakni */
    Kigyodarab* uj = (Kigyodarab*) malloc(sizeof(Kigyodarab));

/* pointerek beláncolása */
    uj->elozo = NULL; //előtte nincs semmi
    uj->kov = melyik_kigyo->feje;
    melyik_kigyo->feje->elozo = uj;

/* ez lesz az új fej */
    melyik_kigyo->feje = uj;

/* irány beállítása - ezt itt már biztos tudjuk, hogy helyes, mert le volt ellenőrizve */
    melyik_kigyo->merre_megy = merre;

/* az új fej mindig az előző fej után a megfelelő irányban van */
    switch(melyik_kigyo->merre_megy){
        case jobbra:
            (uj->hely).x = (uj->kov->hely).x + 1;
            (uj->hely).y = (uj->kov->hely).y;
            break;

        case balra:
            (uj->hely).x = (uj->kov->hely).x - 1;
            (uj->hely).y = (uj->kov->hely).y;
            break;

        case fel:
            (uj->hely).x = ((uj->kov)->hely).x;
            (uj->hely).y = ((uj->kov)->hely).y + 1;
            break;

        case le:
            (uj->hely).x = ((uj->kov)->hely).x;
            (uj->hely).y = ((uj->kov)->hely).y - 1;
            break;
    }

/* a hol_van tömböt frissítjük */
    if((melyik_kigyo->hol_van)[(uj->hely).x][(uj->hely).y] == zoldseg) kigyo_evett(melyik_kigyo,zoldseg);
    if((melyik_kigyo->hol_van)[(uj->hely).x][(uj->hely).y] == hus) kigyo_evett(melyik_kigyo,hus);
    (melyik_kigyo->hol_van)[(uj->hely).x][(uj->hely).y] = kigyo;
    (melyik_kigyo->merre_nez)[(uj->hely).x][(uj->hely).y] = merre;
    uj = melyik_kigyo->farka;
    (melyik_kigyo->hol_van)[(uj->hely).x][(uj->hely).y] = semmi;

/* a farka pointert átállítjuk, a régi farkat felszabadítjuk */
    melyik_kigyo->farka = uj->elozo;
    free(uj);
    melyik_kigyo->farka->kov = NULL;


    Kigyodarab* it = melyik_kigyo->feje->kov;
   // if(kigyo_evett(melyik_kigyo)) kigyo_uj_kaja(melyik_kigyo);
   //for(Kigyodarab *it = melyik_kigyo->feje->kov; it != melyik_kigyo->farka->elozo; it = it->kov){
        if(it->elozo->hely.x + 1 == it->kov->hely.x && it->elozo->hely.y - 1 == it->kov->hely.y){
            melyik_kigyo->merre_nez[it->hely.x][it->hely.y] = feljobbra;
        }
        if(it->elozo->hely.x + 1 == it->kov->hely.x && it->elozo->hely.y + 1 == it->kov->hely.y){
            melyik_kigyo->merre_nez[it->hely.x][it->hely.y] = lejobbra;
        }
        if(it->elozo->hely.x - 1 == it->kov->hely.x && it->elozo->hely.y + 1 == it->kov->hely.y){
            melyik_kigyo->merre_nez[it->hely.x][it->hely.y] = felbalra;
        }
        if(it->elozo->hely.x - 1 == it->kov->hely.x && it->elozo->hely.y - 1 == it->kov->hely.y){
            melyik_kigyo->merre_nez[it->hely.x][it->hely.y] = lebalra;
        }
   //}

}


void kigyo_rajzol(Kigyo* melyik_kigyo, SDL_Renderer* renderer){

printf("kigyo_rajzol\n");
//SDL_Texture *fej = (SDL_Texture*)IMG_LoadTexture(renderer, "attilafej.png");
//SDL_Texture *kisfej = (SDL_Texture*)IMG_LoadTexture(renderer, "attilafejkicsi.png");


    SDL_Texture *fej = (SDL_Texture*)IMG_LoadTexture(renderer, "attilafej.png");
    SDL_Texture *kisfej = (SDL_Texture*)IMG_LoadTexture(renderer, "attilafejkicsi.png");
    SDL_Texture *shit = (SDL_Texture*)IMG_LoadTexture(renderer, "shit.png");


    SDL_Rect src = { 0, 0, 80, 80 };

    for(int i=1;i<pszelesseg+1;++i){
        for(int j=1;j<pmagassag+1;++j){
            if(melyik_kigyo->hol_van[i][j] == kigyo){

                SDL_Rect dest = {(i-1)*80, (j-1)*80, 80, 80 };

                switch(melyik_kigyo->merre_nez[i][j]){
                    case jobbra:

                        SDL_RenderCopyEx(renderer,fej,&src,&dest,90,NULL,SDL_FLIP_NONE);
                        //filledCircleRGBA(renderer, (i-1)*80, (j-1)*80, 40, 255, 0, 0, 255);

                        break;

                    case balra:
                        SDL_RenderCopyEx(renderer,fej,&src,&dest,270,NULL,SDL_FLIP_NONE);
                        //filledCircleRGBA(renderer, (i-1)*80, (j-1)*80, 40, 255, 0, 0, 255);
                        break;

                    case fel:
                        SDL_RenderCopyEx(renderer,fej,&src,&dest,180,NULL,SDL_FLIP_NONE);
                        //filledCircleRGBA(renderer, (i-1)*80, (j-1)*80, 40, 255, 0, 0, 255);
                        break;

                    case le:
                        SDL_RenderCopyEx(renderer,fej,&src,&dest,0,NULL,SDL_FLIP_NONE);
                        //filledCircleRGBA(renderer, (i-1)*80, (j-1)*80, 40, 255, 0, 0, 255);
                        break;

                    case feljobbra:
                        SDL_RenderCopyEx(renderer,kisfej,&src,&dest,225,NULL,SDL_FLIP_NONE);
                        //filledCircleRGBA(renderer, (i-1)*80, (j-1)*80, 40, 255, 0, 0, 255);
                        break;

                    case lejobbra:
                        SDL_RenderCopyEx(renderer,kisfej,&src,&dest,315,NULL,SDL_FLIP_NONE);
                        //filledCircleRGBA(renderer, (i-1)*80, (j-1)*80, 40, 255, 0, 0, 255);
                        break;

                    case lebalra:
                        SDL_RenderCopyEx(renderer,kisfej,&src,&dest,135,NULL,SDL_FLIP_NONE);
                        //filledCircleRGBA(renderer, (i-1)*80, (j-1)*80, 40, 255, 0, 0, 255);
                        break;

                    case felbalra:
                        SDL_RenderCopyEx(renderer,kisfej,&src,&dest,45,NULL,SDL_FLIP_NONE);
                        //filledCircleRGBA(renderer, (i-1)*80, (j-1)*80, 40, 255, 0, 0, 255);
                        break;
                }

                /*SDL_Texture *fej = (SDL_Texture*)IMG_LoadTexture(renderer, "attilafej.png");
                SDL_Rect src = { 0, 0, 80, 80 };
                SDL_Rect dest = {(i-1)*80, (j-1)*80, 80, 80 };
                SDL_RenderCopy(renderer, fej, &src, &dest);*/
            }

            if(melyik_kigyo->hol_van[i][j] == zoldseg){

                //SDL_Rect src = { 0, 0, 80, 80 };
                SDL_Rect dest = {(i-1)*80, (j-1)*80, 80, 80 };
                SDL_RenderCopy(renderer, shit, &src, &dest);
                //filledCircleRGBA(renderer, (i-1)*80+40, (j-1)*80+40, 40, 0, 255, 0, 255);
            }

            if(melyik_kigyo->hol_van[i][j] == hus){
                //filledCircleRGBA(renderer, (i-1)*80+40, (j-1)*80+40, 40, 255, 0, 0, 255);
                 SDL_Texture *hus = (SDL_Texture*)IMG_LoadTexture(renderer, "angery.png");
                //SDL_Rect src = { 0, 0, 80, 80 };
                SDL_Rect dest = {(i-1)*80, (j-1)*80, 80, 80 };
                SDL_RenderCopy(renderer, hus, &src, &dest);
                SDL_DestroyTexture(hus);
            }

        }
    }
    SDL_DestroyTexture(fej);
    SDL_DestroyTexture(kisfej);
    SDL_DestroyTexture(shit);



//printf("(x=%d\ty=%d)\n",melyik_kigyo->feje->hely.x, melyik_kigyo->feje->hely.y);
}



bool kigyo_valid_mozgas(Kigyo* melyik_kigyo, Irany merre){
printf("kigyo_valid_mozgas\n");

    if(melyik_kigyo->meghalt) return false;
    /*if(merre == le && melyik_kigyo->feje->hely.x == melyik_kigyo->feje->kov->hely.x && melyik_kigyo->feje->hely.y == melyik_kigyo->feje->kov->hely.y - 1) return true;
    if(merre == fel && melyik_kigyo->feje->hely.x == melyik_kigyo->feje->kov->hely.x && melyik_kigyo->feje->hely.y == melyik_kigyo->feje->kov->hely.y + 1) return true;
    if(merre == balra && melyik_kigyo->feje->hely.x == melyik_kigyo->feje->kov->hely.x - 1 && melyik_kigyo->feje->hely.y == melyik_kigyo->feje->kov->hely.y) return true;
    if(merre == jobbra && melyik_kigyo->feje->hely.x == melyik_kigyo->feje->kov->hely.x + 1 && melyik_kigyo->feje->hely.y == melyik_kigyo->feje->kov->hely.y) return true;
    if(merre == le && melyik_kigyo->feje->hely.x == melyik_kigyo->feje->kov->hely.x && melyik_kigyo->feje->hely.y == melyik_kigyo->feje->kov->hely.y - 1) return true;
    if(merre == fel && melyik_kigyo->feje->hely.x == melyik_kigyo->feje->kov->hely.x && melyik_kigyo->feje->hely.y == melyik_kigyo->feje->kov->hely.y + 1) return true;
    if(merre == balra && melyik_kigyo->feje->hely.x == melyik_kigyo->feje->kov->hely.x - 1 && melyik_kigyo->feje->hely.y == melyik_kigyo->feje->kov->hely.y) return true;
    if(merre == jobbra && melyik_kigyo->feje->hely.x == melyik_kigyo->feje->kov->hely.x + 1 && melyik_kigyo->feje->hely.y == melyik_kigyo->feje->kov->hely.y) return true;*/


    /*if(melyik_kigyo->merre_megy == jobbra && merre == fel &&  melyik_kigyo->feje->hely.x == melyik_kigyo->feje->kov->hely.x && melyik_kigyo->feje->hely.y == melyik_kigyo->feje->kov->hely.y - 1) {melyik_kigyo->merre_megy =fel;return false;}
    if(melyik_kigyo->merre_megy == jobbra && merre == le &&  melyik_kigyo->feje->hely.x == melyik_kigyo->feje->kov->hely.x && melyik_kigyo->feje->hely.y == melyik_kigyo->feje->kov->hely.y + 1) {melyik_kigyo->merre_megy =le;return false;}
    if(melyik_kigyo->merre_megy == balra && merre == fel &&  melyik_kigyo->feje->hely.x == melyik_kigyo->feje->kov->hely.x && melyik_kigyo->feje->hely.y == melyik_kigyo->feje->kov->hely.y - 1){ melyik_kigyo->merre_megy =fel;return false;}
    if(melyik_kigyo->merre_megy == balra && merre == le &&  melyik_kigyo->feje->hely.x == melyik_kigyo->feje->kov->hely.x && melyik_kigyo->feje->hely.y == melyik_kigyo->feje->kov->hely.y + 1) {melyik_kigyo->merre_megy =le;return false;}

*/

    if(merre == fel && melyik_kigyo->merre_megy == le)return false;
    if(merre == le && melyik_kigyo->merre_megy == fel)return false;
    if(merre == balra && melyik_kigyo->merre_megy == jobbra)return false;
    if(merre == jobbra && melyik_kigyo->merre_megy == balra)return false;

    /* játék végét okozó mozdulatok: */
    if(merre == le && melyik_kigyo->feje->hely.y <= 1) {melyik_kigyo->meghalt = true; return false;}
    if(merre == fel && melyik_kigyo->feje->hely.y >= pmagassag) {melyik_kigyo->meghalt = true; return false;}
    if(merre == balra && melyik_kigyo->feje->hely.x <= 1) {melyik_kigyo->meghalt = true; return false;}
    if(merre == jobbra && melyik_kigyo->feje->hely.x >= pszelesseg) {melyik_kigyo->meghalt = true; return false;}

    if(merre == fel && melyik_kigyo->hol_van[melyik_kigyo->feje->hely.x][melyik_kigyo->feje->hely.y + 1] == kigyo) {melyik_kigyo->meghalt = true; return false;}
    if(merre == le && melyik_kigyo->hol_van[melyik_kigyo->feje->hely.x][melyik_kigyo->feje->hely.y - 1] == kigyo) {melyik_kigyo->meghalt = true; return false;}
    if(merre == balra && melyik_kigyo->hol_van[melyik_kigyo->feje->hely.x -1][melyik_kigyo->feje->hely.y] == kigyo) {melyik_kigyo->meghalt = true; return false;}
    if(merre == jobbra && melyik_kigyo->hol_van[melyik_kigyo->feje->hely.x + 1][melyik_kigyo->feje->hely.y] == kigyo) {melyik_kigyo->meghalt = true; return false;}

    if(merre == melyik_kigyo->merre_megy) true;
    return true;
}

void kigyo_cb(Kigyo* melyik_kigyo, Felh_event gomb){
printf("kigyo_cb\n");
    /*switch(gomb){
        case le_elenged: if(kigyo_valid_mozgas(melyik_kigyo,le)) kigyo_halad(melyik_kigyo,le); break;
        case fel_elenged: if(kigyo_valid_mozgas(melyik_kigyo,fel)) kigyo_halad(melyik_kigyo,fel); break;
        case bal_elenged: if(kigyo_valid_mozgas(melyik_kigyo,balra)) kigyo_halad(melyik_kigyo,balra); break;
        case jobb_elenged: if(kigyo_valid_mozgas(melyik_kigyo,jobbra)) kigyo_halad(melyik_kigyo,jobbra); break;
        case utemezett: if(!kigyo_valid_mozgas(melyik_kigyo,melyik_kigyo->merre_megy)){ kigyo_halad(melyik_kigyo,melyik_kigyo->merre_megy); break;}
        case escape_megnyom:
        case escape_elenged:
        default: break;
    }*/
    switch(gomb){
        case utemezett: if(kigyo_valid_mozgas(melyik_kigyo,melyik_kigyo->merre_megy)){ kigyo_halad(melyik_kigyo,melyik_kigyo->merre_megy); melyik_kigyo->iranyban = false;break;}
        case le_megnyom: if(!(melyik_kigyo->iranyban) && kigyo_valid_mozgas(melyik_kigyo,le) ) {melyik_kigyo->merre_megy = le; melyik_kigyo->iranyban = true; break;}
        case fel_megnyom: if(!(melyik_kigyo->iranyban) && kigyo_valid_mozgas(melyik_kigyo,fel)) {melyik_kigyo->merre_megy = fel; melyik_kigyo->iranyban = true; break;}
        case bal_megnyom: if(!(melyik_kigyo->iranyban) && kigyo_valid_mozgas(melyik_kigyo,balra)) {melyik_kigyo->merre_megy = balra; melyik_kigyo->iranyban = true; break;}
        case jobb_megnyom: if(!(melyik_kigyo->iranyban) && kigyo_valid_mozgas(melyik_kigyo,jobbra)){ melyik_kigyo->merre_megy = jobbra;melyik_kigyo->iranyban = true; break;}
        case le_elenged: if(!(melyik_kigyo->iranyban) && kigyo_valid_mozgas(melyik_kigyo,le) ) {melyik_kigyo->merre_megy = le; melyik_kigyo->iranyban = true; break;}
        case fel_elenged: if(!(melyik_kigyo->iranyban) && kigyo_valid_mozgas(melyik_kigyo,fel)) {melyik_kigyo->merre_megy = fel; melyik_kigyo->iranyban = true; break;}
        case bal_elenged: if(!(melyik_kigyo->iranyban) && kigyo_valid_mozgas(melyik_kigyo,balra)) {melyik_kigyo->merre_megy = balra; melyik_kigyo->iranyban = true; break;}
        case jobb_elenged: if(!(melyik_kigyo->iranyban) && kigyo_valid_mozgas(melyik_kigyo,jobbra)){ melyik_kigyo->merre_megy = jobbra;melyik_kigyo->iranyban = true; break;}
        case escape_megnyom:
        case escape_elenged:
        default: break;
    }
}



bool kigyo_evett(Kigyo* melyik_kigyo, Palyaelem mit){
printf("kigyo_evett\n");

    if(mit == zoldseg){
        Mix_Chunk *hang1 = Mix_LoadWAV("hang1.wav");
        Mix_PlayChannel(-1,hang1,0);
        kigyo_nyujt(melyik_kigyo);
        kigyo_uj_kaja(melyik_kigyo, zoldseg);
        //Mix_FreeChunk(hang1);
        return true;
    }
    if(mit == hus){
        Mix_Chunk *hang2 = Mix_LoadWAV("hang2.wav");
        Mix_PlayChannel(-1,hang2,0);
        if(melyik_kigyo->hossz >= 5){
            kigyo_zsugorit(melyik_kigyo);
        }
        kigyo_uj_kaja(melyik_kigyo, hus);
        //Mix_FreeChunk(hang2);
        return true;
    }

    return false;
}

void kigyo_nyujt(Kigyo* melyik_kigyo){
printf("kigyo_nyujt\n");
    ++(melyik_kigyo->hossz);
    int vx,vy,x,y;
    vx = melyik_kigyo->farka->elozo->hely.x - melyik_kigyo->farka->hely.x;
    vy = melyik_kigyo->farka->elozo->hely.y - melyik_kigyo->farka->hely.y;
    x = melyik_kigyo->farka->hely.x - vx;
    y = melyik_kigyo->farka->hely.y - vy;
    Kigyodarab* uj = (Kigyodarab*) malloc(sizeof(Kigyodarab));
    uj->kov = NULL;
    uj->elozo = melyik_kigyo->farka;
    uj->hely.x = x;
    uj->hely.y = y;
    melyik_kigyo->farka->kov = uj;
    melyik_kigyo->farka = uj;

}

void kigyo_zsugorit(Kigyo* melyik_kigyo){
printf("kigyo_zsugorit\n");
    /*melyik_kigyo->hossz -= 2;
    Kigyodarab* uj = melyik_kigyo->farka;
    melyik_kigyo->hol_van[uj->hely.x][uj->hely.y] = semmi;
    melyik_kigyo->farka = uj->elozo;
    free(uj);
    uj = melyik_kigyo->farka;
    melyik_kigyo->hol_van[uj->hely.x][uj->hely.y] = semmi;
    melyik_kigyo->farka = uj->elozo;
    free(uj);
    melyik_kigyo->farka->kov = NULL;*/
    Kigyodarab* uj;
    for(int i=0; i < melyik_kigyo->hossz / 2; ++i){
        uj = melyik_kigyo->farka;
        melyik_kigyo->hol_van[uj->hely.x][uj->hely.y] = semmi;
        melyik_kigyo->farka = uj->elozo;
        free(uj);
    }
    melyik_kigyo->hossz = melyik_kigyo->hossz - melyik_kigyo->hossz / 2;
}

void kigyo_uj_kaja(Kigyo* melyik_kigyo, Palyaelem uj){
printf("kigyo_uj_kaja\n");
    if(uj == zoldseg){
    int kaja_x = rand()%9 + 1;
    int kaja_y = rand()%9 + 1;

        while(melyik_kigyo->hol_van[kaja_x][kaja_y] != semmi || (kaja_x == melyik_kigyo->feje->hely.x && kaja_y == melyik_kigyo->feje->hely.y) || (kaja_x == melyik_kigyo->farka->hely.x && kaja_y == melyik_kigyo->farka->hely.y)){
            kaja_x = rand()%9 + 1;
            kaja_y = rand()%9 + 1;
        }
        melyik_kigyo->hol_van[kaja_x][kaja_y] = zoldseg;
    }
    if(uj == hus){
        int mereg_x = rand()%9 + 1;
        int mereg_y = rand()%9 + 1;

        while(melyik_kigyo->hol_van[mereg_x][mereg_y] != semmi || (mereg_x == melyik_kigyo->feje->hely.x && mereg_y == melyik_kigyo->feje->hely.y) || (mereg_x == melyik_kigyo->farka->hely.x && mereg_y == melyik_kigyo->farka->hely.y)){
            mereg_x = rand()%9 + 1;
            mereg_y = rand()%9 + 1;
        }
        melyik_kigyo->hol_van[mereg_x][mereg_y] = hus;
    }


//printf("kaja_x = %d\tkaja_y = %d\n",kaja_x, kaja_y);
}

void kigyo_felszabadit(Kigyo* melyik_kigyo){
printf("kigyo_felszabadit\n");
    for(int i=0;i<pszelesseg+2;++i){
        for(int j=0;j<pmagassag+2;++j){
            (melyik_kigyo->hol_van)[i][j]=semmi;
        }
    }
    Kigyodarab* lemarado = melyik_kigyo->feje;
    for(int i=0;i < melyik_kigyo->hossz - 1; ++i){
        Kigyodarab* mozgo = lemarado->kov;
        free(lemarado);
        lemarado = mozgo;
        mozgo = mozgo->kov;
    }
}

/*void kigyo_magatol_megy(Kigyo* melyik_kigyo){
    kigyo_halad(melyik_kigyo,melyik_kigyo->merre_megy);
}*/
