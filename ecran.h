#ifndef ECRAN_H
#define ECRAN_H
#include <inttypes.h>

uint16_t *ptr_mem(uint32_t lig, uint32_t col);
void ecrit_car(uint32_t lig, uint32_t col, char c, char color);
void efface_ecran(void);
void place_curseur(uint32_t lig, uint32_t col);
void traite_car(char c);
void defilement(void);
void console_putbytes(const char *s, int len);

void ecrit_temps(char *s, int len);
void traitant_IT_32(void);
void tic_PIT(void);
void init_traitant_IT(void (*traitant)(void), uint32_t num_IT);
void setClockFreq(void);
void unmaskIRQ0(void);

extern const int FREQ_CLOCK;
extern const int QUARTZ;

void traitant_IT_40(void);
void ecrit_date(char *s);
uint8_t lit_CMOS(uint8_t reg);
void affiche_date(void);
void regle_frequence_RTC(void);
void bcd_to_decimal(uint8_t *val);
void unmaskIRQ8(void);

#endif