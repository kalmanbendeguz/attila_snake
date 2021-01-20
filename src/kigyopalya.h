#ifndef KIGYOPALYA_H_INCLUDED
#define KIGYOPALYA_H_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define pszelesseg 9
#define pmagassag 9

typedef enum Felh_event{
    le_elenged,
    le_megnyom,
    fel_elenged,
    fel_megnyom,
    bal_elenged,
    bal_megnyom,
    jobb_elenged,
    jobb_megnyom,
    escape_elenged,
    escape_megnyom,
    utemezett
}Felh_event;

typedef enum Irany{
    fel,le,jobbra,balra, feljobbra,lejobbra,lebalra, felbalra
}Irany;

typedef enum Palyaelem{
    semmi, kigyo, zoldseg, hus
}Palyaelem;

typedef struct Koord{
    int x;
    int y;
}Koord;

typedef struct Kigyodarab{
    Koord hely;
    struct Kigyodarab* kov;
    struct Kigyodarab* elozo;
}Kigyodarab;

typedef struct Kigyo{
    int hossz;
    Irany merre_megy;
    Palyaelem hol_van[pszelesseg+2][pmagassag+2];
    Irany merre_nez[pszelesseg+2][pmagassag+2];
    Kigyodarab* feje;
    Kigyodarab* farka;
    bool meghalt;
    bool iranyban;
}Kigyo;

void kigyo_init(Kigyo* melyik_kigyo);
void kigyo_halad(Kigyo* melyik_kigyo, Irany merre);
void kigyo_rajzol(Kigyo* melyik_kigyo, SDL_Renderer* renderer);
bool kigyo_valid_mozgas(Kigyo* melyik_kigyo, Irany merre);
void kigyo_cb(Kigyo* melyik_kigyo, Felh_event gomb);
bool kigyo_evett(Kigyo* melyik_kigyo, Palyaelem mit);
void kigyo_nyujt(Kigyo* melyik_kigyo);
void kigyo_zsugorit(Kigyo* melyik_kigyo);
void kigyo_uj_kaja(Kigyo* melyik_kigyo, Palyaelem uj);
void kigyo_felszabadit(Kigyo* melyik_kigyo);
#endif // KIGYOPALYA_H_INCLUDED
