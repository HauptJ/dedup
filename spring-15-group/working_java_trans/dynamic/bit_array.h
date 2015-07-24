#ifndef __BIT_ARRAY__
#define __BIT_ARRAY__

#define ZERO 0
#define BIG 1
#define LITTLE 2

typedef struct {unsigned char* byte;int size;} bits;
 

// bit initializations
bits* bits_init(int);
bits* bits_initlong();
bits* bits_initbits(bits*, int);
bits* bits_initstring(char*);
bits* bits_initbytes(char[], int);
int endianness();

// bit comparisons
int lte(bits*, bits*); //less than equal
int gte(bits*, bits*); //greater than equal
int gt(bits*, bits*); //greater than
int lt(bits*, bits*); //less than
int eq(bits*, bits*); //equal

// bit ops (operations)
bits* and(bits*, bits*);
bits* or(bits*, bits*);
bits* xor(bits*, bits*);
bits* shiftl_bits(bits*, int); //shift left
bits* shiftl_byte(bits*, int); //shift left
bits* shiftl(bits*, int); //shift left
bits* shiftr(bits*, int); //shift right

// debug - used to print test results
void bit_array_print_report();
#endif
