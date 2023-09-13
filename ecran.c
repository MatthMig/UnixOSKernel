#include "ecran.h"
#include "cpu.h"
#include <stdbool.h>
#include "inttypes.h"
#include "stdio.h"
#include "segment.h"

const int FREQ_CLOCK = 50;
const int QUARTZ = 0x1234DD;

/*uint16_t* ptr_mem(uint32_t lig, uint32_t col) {
    uint16_t* mem_ptr = (uint16_t*)0xB8000;  // Adresse de début de la mémoire vidéo
    uint16_t offset =  (lig * 80 + col);  // Calcul de l'offset dans la mémoire vidéo

    return mem_ptr + offset;  // Retourne le pointeur sur la case mémoire correspondante
}*/

/*void ecrit_car(uint32_t lig, uint32_t col, char c, char color){
    uint16_t * addr = ptr_mem(lig, col);
    *addr = 0;
    *addr |= c;
    *addr |= color << 8;
    *addr &= 0x7fff;
}
*/

void efface_ecran(void) {
    uint32_t i;

    for (i = 0; i < 80 * 25; i++) {
        ecrit_car(i / 80, i % 80, ' ',0x0F);
    }
}

/*void place_curseur(uint32_t lig, uint32_t col) {
    uint16_t pos = (uint16_t)(lig * 80 + col);  // Calcul de la position du curseur

    outb(0x0F, 0x3D4);  // Envoi de la commande 0x0F au port 0x3D4
    outb((uint8_t)pos, 0x3D5);  // Envoi de la position du curseur (partie basse) au port 0x3D5
    outb(0x0E, 0x3D4);  // Envoi de la commande 0x0E au port 0x3D4
    outb((uint8_t)(pos >> 8), 0x3D5);  // Envoi de la position du curseur (partie haute) au port 0x3D5
}*/

void traite_car(char c) {
    static uint32_t lig = 0;  // Ligne courante
    static uint32_t col = 0;  // Colonne courante

    switch (c) {
        case '\n':  // Retour à la ligne
            lig++;
            col = 0;
            break;
        case '\t':  // Tabulation
            col += 8 - (col % 8);
            break;
        case '\b':  // Retour arrière
            if (col > 0) {
                col--;
            }
            break;
        case '\f':  // Effacement de l'écran
            efface_ecran();
            lig = 0;
            col = 0;
            break;
        case '\r':  // Retour au début de la ligne
            col = 0;
            break;
        default:  // Caractère normal
            ecrit_car(lig, col, c,0x0F);
            col++;
            break;
    }

    if (col >= 80) {  // Si on dépasse la fin de la ligne
        lig++;
        col = 0;
    }

    if (lig >= 25) {  // Si on dépasse la fin de l'écran
        efface_ecran();
        lig = 0;
        col = 0;
    }

    place_curseur(lig, col);  // Placement du curseur
}

void defilement(void) {
    uint32_t i;

    for (i = 0; i < 80 * 24; i++) {
        ecrit_car(i / 80, i % 80, *ptr_mem((i + 80) / 80, (i + 80) % 80),0x0F);
    }

    for (i = 0; i < 80; i++) {
        ecrit_car(24, i, ' ',0x0F);
    }
}

void console_putbytes(const char *s, int len) {
    int i;

    for (i = 0; i < len; i++) {
        traite_car(s[i]);
    }
}

void ecrit_temps(char *s, int len) {
    for (int i = 1; i < len; i++) {
        ecrit_car(0, 80-i, s[len-i-1],0x0F);
    }
}

void ecrit_date(char *s) {
    for (int i = 1; s[i] != '\0'; i++) {
        ecrit_car(0, 0, s[i],0x0F);
    }
}

// Traitant de l'interruption 32 qui affiche à l'écran le temps écoulé
// depuis le démarrage du système
void tic_PIT(void) {
    outb(0x20, 0x20);

    static int cpt = 0;
    int tailleLabel = 9;

    cpt ++;
    if(cpt % FREQ_CLOCK == 0) {
        int secondes = (cpt/FREQ_CLOCK) % 60;
        int minutes = ((cpt/FREQ_CLOCK) % (60*60)) / 60;
        int heures = (cpt/FREQ_CLOCK) / (60*60);

        char tempsLabel[tailleLabel];
        // tempsLabel: "HH:MM:SS" 
        sprintf(tempsLabel, "%02d:%02d.%02d", heures, minutes, secondes);
        ecrit_temps(tempsLabel, tailleLabel);
    }    
}

void init_traitant_IT(void (*traitant)(void), uint32_t num_IT) {
    uint32_t *adresse = (uint32_t *)0x1000 + num_IT*2;

    uint32_t premierMot = (KERNEL_CS & 0xFFFF) << 16;
    premierMot |= (uint32_t)traitant & 0xFFFF;
    
    uint32_t deuxiemeMot = (uint32_t)traitant & 0xFFFF0000;
    deuxiemeMot |= 0x8E00;

    *adresse = premierMot;
    *(adresse+1) = deuxiemeMot;
}

/*void setClockFreq(void) {
    outb(0x34, 0x43);
    outb((QUARTZ / FREQ_CLOCK) & 0xFF, 0x40);
    outb(((QUARTZ / FREQ_CLOCK) >> 8) & 0xFF, 0x40);
}*/

void unmaskIRQ0(void) {
    uint8_t mask = inb(0x21);
    mask &= 0xFE;
    outb(mask, 0x21);
}

void unmaskIRQ8(void) {
    uint8_t mask = inb(0x21);
    mask &= 0x7B;
    outb(mask, 0x21);
}

uint8_t lit_CMOS(uint8_t reg) {
    uint8_t res;
    outb(0x80 | reg, 0x70);
    res = inb(0x71);
    bcd_to_decimal(&res);
    return res;
}

void bcd_to_decimal(uint8_t *val) {
    *val = (*val & 0x0F) + ((*val >> 4) * 10);
}

void affiche_date(void) {

    uint8_t secondes = lit_CMOS(0);
    uint8_t minutes = lit_CMOS(2);
    uint8_t heures = lit_CMOS(4);
    uint8_t jour = lit_CMOS(6);
    uint8_t num_jour = lit_CMOS(7);
    uint8_t mois = lit_CMOS(8);
    uint8_t annee = lit_CMOS(9);
    uint8_t centenaire = lit_CMOS(0x32);

    char label_jour[9];
    switch(jour) {
        case 0:
            sprintf(label_jour, "Dimanche");
            break;
        case 1:
            sprintf(label_jour, "Lundi");
            break;
        case 2:
            sprintf(label_jour, "Mardi");
            break;
        case 3:
            sprintf(label_jour, "Mercredi");
            break;
        case 4:
            sprintf(label_jour, "Jeudi");
            break;
        case 5:
            sprintf(label_jour, "Vendredi");
            break;
        case 6:
            sprintf(label_jour, "Samedi");
            break;
    }
    char date[29];
    sprintf(date, "%s %02d/%02d/%02d %02d:%02d:%02d", label_jour, num_jour, mois, centenaire*100+annee, heures, minutes, secondes);
    ecrit_date(date);
}

void regle_frequence_RTC(void) {
    // Reglage de la frequence du RTC
    outb(0x8A, 0x70);
    uint8_t v = inb(0x71);
    outb(0x8A, 0x70);
    outb((v & 0xF0) | 15, 0x71);
    
    // Choix du type de signal 
    outb(0x8B, 0x70);
    v = inb(0x71);
    outb(0x8B, 0x70);
    outb(v | 0x40, 0x71);
}