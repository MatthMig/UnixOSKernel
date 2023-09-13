.text
# cette directive sert a rendre l’etiquette publique
    .globl traitant_IT_32
# debut du traitant
traitant_IT_32:
# sauvegarde des registres importants
    pushl %eax
    pushl %edx
    pushl %ecx
# appel a la fonction C realisant le traitant
    call tic_PIT
# restauration des registres importants
    popl %ecx
    popl %edx
    popl %eax
# fin du traitant
    iret

.text
# cette directive sert a rendre l’etiquette publique
    .globl traitant_IT_40
# debut du traitant
traitant_IT_40:
# sauvegarde des registres importants
    pushl %eax
    pushl %edx
    pushl %ecx
# acquite interruptions
    movb $0x20, %al
    movw $0x20, %dx
    outb %al, %dx
    movw $0xA0, %dx
    outb %al, %dx
# signaler qu'on a reçu le signal
    movb $0x70, %al
    movw $0x8C, %dx
    outb %al, %dx
    movw $0x71, %dx
    inb %dx
# appel a la fonction C realisant le traitant
    call affiche_date
# restauration des registres importants
    popl %ecx
    popl %edx
    popl %eax
# fin du traitant
    iret