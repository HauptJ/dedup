#ifndef __BIG_INTEGER_H__
#define __BIG_INTEGER_H__

#define BYTES 8  // default size of integer/window in bytes

//endianness (for initialization via integer_initval only)
#define BIG 1
#define LITTLE 2

typedef struct {unsigned char byte[BYTES];} integer;

//integer initialization
integer integer_init();
integer integer_initones();
integer integer_initval(unsigned long int);
integer integer_initint(integer);
int endianness();
unsigned int swapbytes(unsigned int);
void integer_free(integer);

//integer comparisons
int integer_lte(integer, integer);
int integer_gte(integer, integer);
int integer_gt(integer, integer);
int integer_lt(integer, integer);
int integer_eq(integer, integer);

//integer math
integer integer_mult(integer, integer);
integer integer_mult_slow(integer, integer);
integer integer_mod(integer, integer);
integer integer_add(integer, integer, int*);
integer integer_sub(integer, integer);

//bitwise operation
integer integer_shiftl(integer);
integer integer_shiftl_times(integer, int);
integer integer_and(integer, integer);
integer integer_or(integer, integer);
integer integer_xor(integer, integer);


//polynomial math
integer integer_poly_mult(integer a, integer b);
int integer_poly_degree(integer a);
int integer_poly_hasdegree(integer a, int deg);
integer integer_poly_mod(integer a, integer b);
integer integer_poly_gcd(integer a, integer b);
integer integer_poly_modPow(integer base, integer exponent, integer modulus);


//more integer math
integer integer_gcd(integer, integer);
integer integer_genrandprime();
int integer_isprime(integer);

//misc
void integer_printhex(integer);
void integer_gen_test_print();

#endif


