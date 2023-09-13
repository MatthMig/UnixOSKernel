#include <cpu.h>
#include <inttypes.h>
#include <stdio.h>
#include "ecran.h"

// on peut s'entrainer a utiliser GDB avec ce code de base
// par exemple afficher les valeurs de x, n et res avec la commande display

// une fonction bien connue
uint32_t fact(uint32_t n)
{
    uint32_t res;
    if (n <= 1) {
        res = 1;
    } else {
        res = fact(n - 1) * n;
    }
    return res;
}

void kernel_start(void)
{
    // initialisations
        efface_ecran();
        place_curseur(0, 0);
        
        setClockFreq();
        unmaskIRQ0();
        init_traitant_IT(traitant_IT_32,32);

        regle_frequence_RTC();
        unmaskIRQ8();
        init_traitant_IT(traitant_IT_40,40);

    // démasquage des interruptions externes
        sti();
        

    // boucle d'attente
        while (1) hlt();    
}
