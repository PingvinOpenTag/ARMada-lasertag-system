#ifndef __CIPHER_H
#define __CIPHER_H

#include <stdio.h>
#include <stdint.h>

uint32_t rol(uint32_t base, uint32_t shift);
void xtea2_encipher(unsigned int num_rounds,uint32_t *v, uint32_t const *k);
void xtea2_decipher(unsigned int num_rounds,uint32_t *v, uint32_t const *k);
void get_uid(volatile uint32_t *text);


#endif /* __CIPHER_H */
