#ifndef __BIT_ARRAY__
#define __BIT_ARRAY__

#define BYTES 130 //up to 32 good
#define ZERO 0
#define BIG 1
#define LITTLE 2

typedef struct {unsigned char byte[BYTES];} bits;
//typedef struct {unsigned char* byte;int bytes;} big_bits;


// bit initializations
bits bits_init();
bits bits_initones();
bits bits_initlong();
bits bits_initbits(bits);
bits bits_initbytes(char[], int);
//big_bits bits_initbig(char*,int);
int endianness();

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
void bit_array_print_report();
#endif
