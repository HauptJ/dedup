#ifndef __BIT_ARRAY__
#define __BIT_ARRAY__

#define BYTES 8
#define ZERO 0
#define BIG 1
#define LITTLE 2

typedef struct {unsigned char byte[BYTES];} bits;

// bit initializations
bits bits_init();
bits bits_initones();
bits bits_initlong();
bits bits_initbits(bits);
int endianness();
unsigned int swapbytes(unsigned int);

// bit comparisons
int lte(bits, bits);
int gte(bits, bits);
int gt(bits, bits);
int lt(bits, bits);
int eq(bits, bits);

// bit ops
bits and(bits, bits);
bits or(bits, bits);
bits xor(bits, bits);
bits shiftl(bits, int);
bits shiftr(bits, int);

// debug
void print_hex(bits);
#endif
