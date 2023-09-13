/*
uint16_t* ptr_mem(uint32_t lig, uint32_t col) {
    uint16_t* mem_ptr = (uint16_t*)0xB8000;  // Adresse de début de la mémoire vidéo
    uint32_t offset = 2 * (lig * 80 + col);  // Calcul de l'offset dans la mémoire vidéo

    return mem_ptr + offset;  // Retourne le pointeur sur la case mémoire correspondante
}
*/
.text
.globl ptr_mem
ptr_mem:
    // uint32_t lig: %ebp + 8
    // uint32_t col: %ebp + 12
    enter $48, $0
    // uint32_t offset = 2 * (lig * 80 + col);
    movl 8(%ebp), %edx
    movl $80, %eax
    mul %edx
    movl 12(%ebp), %ecx
    addl %ecx, %eax
    addl %eax, %eax
    // uint16_t* mem_ptr = (uint16_t*)0xB8000;
    movl $0xB8000, %edx
    addl %edx, %eax
    // return mem_ptr + offset;
    leave
    ret

/*void ecrit_car(uint32_t lig, uint32_t col, char c, char color){
    uint16_t * addr = ptr_mem(lig, col);
    *addr = 0;
    *addr |= c;
    *addr |= color << 8;
    *addr &= 0x7fff;
}*/
    .text
    .globl ecrit_car
    //lig : %ebp + 8
    //col : %ebp + 12
    //c : %ebp + 16
    //color : %ebp + 20
ecrit_car:
    enter $16, $0
    //uint16_t * addr = ptr_mem(lig, col);
    pushl 12(%ebp)
    pushl 8(%ebp)
    call ptr_mem
    
    //*addr = 0;
    movw $0, (%eax)
    
    //*addr |= c;
    movw 16(%ebp), %dx
    movw %dx, (%eax)

    movw 20(%ebp), %dx
    //color << 8;
    shlw $8, %dx
    //addr |= color << 8;
    orw %dx, (%eax)

    //*addr &= 0xffff;
    andw $0xffff, (%eax) 

    leave
    ret

/*
void place_curseur(uint32_t lig, uint32_t col) {
    uint16_t pos = (uint16_t)(lig * 80 + col);  // Calcul de la position du curseur

    outb(0x0F, 0x3D4);  // Envoi de la commande 0x0F au port 0x3D4
    outb((uint8_t)pos, 0x3D5);  // Envoi de la position du curseur (partie basse) au port 0x3D5
    outb(0x0E, 0x3D4);  // Envoi de la commande 0x0E au port 0x3D4
    outb((uint8_t)(pos >> 8), 0x3D5);  // Envoi de la position du curseur (partie haute) au port 0x3D5
}
*/
.globl place_curseur
place_curseur:
    // uint32_t lig: %ebp + 8
    // uint32_t col: %ebp + 12
    enter $16, $0
    // uint16_t pos = (uint16_t)(lig * 80 + col);
    movl 12(%ebp), %ecx
    movl 8(%ebp), %edx
    movl $80,%eax
    mul %edx
    add %eax, %ecx
    // outb(0x0F, 0x3D4);
    movb $0x0F, %al
    movw $0x3D4, %dx
    outb %al, %dx
    // outb((uint8_t)pos, 0x3D5);
    movb %cl, %al
    movw $0x3D5, %dx
    outb %al, %dx
    outb %al, %dx
    // outb(0x0E, 0x3D4);
    movb $0x0E, %al
    movw $0x3D4, %dx
    outb %al, %dx
    // outb((uint8_t)(pos >> 8), 0x3D5);
    shrb $8, %cl
    movb %cl, %al
    movw $0x3D5, %dx
    outb %al, %dx
    leave
    ret

/*
void setClockFreq(void) {
    outb(0x34, 0x43);
    outb((QUARTZ / FREQ_CLOCK) & 0xFF, 0x40);
    outb(((QUARTZ / FREQ_CLOCK) >> 8) & 0xFF, 0x40);
} */
.globl setClockFreq
setClockFreq:
    enter $0, $0
    // outb(0x34, 0x43);
    movb $0x34, %al
    movw $0x43, %dx
    outb %al, %dx
    // outb((QUARTZ / FREQ_CLOCK) & 0xFF, 0x40);
    movl QUARTZ, %eax
    movl FREQ_CLOCK, %ecx
    movl $0, %edx
    divl %ecx
    movw $0x40, %dx
    outb %al, %dx
    // outb(((QUARTZ / FREQ_CLOCK) >> 8) & 0xFF, 0x40);
    movb %ah, %al
    outb %al, %dx
    leave
    ret