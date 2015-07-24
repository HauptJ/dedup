#ifndef __BIG_INTEGER_H__
#define __BIG_INTEGER_H__

#define BYTES 8  // default size of integer/window in bytes

//endianness (for initialization via integer_initval only)
#define BIG 1
#define LITTLE 2

typedef struct {unsigned char* byte; unsigned short size;} integer;

//integer initialization
integer integer_init();
integer integer_initones();
integer integer_initval(unsigned long int);
integer integer_initint(integer);
int endianness();
unsigned int swapbytes(unsigned int);


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

//bits bits bits
integer integer_shiftl(integer);
integer integer_shiftl_times(integer, int);
integer integer_and(integer, integer);
integer integer_or(integer, integer);
integer integer_xor(integer, integer);



//random generation and testing
integer integer_gcd(integer, integer);

integer integer_genrandprime();
int integer_isprime(integer);

void integer_printhex(integer);

//barf
void integer_gen_test_print();




/*
def is_prime(n):
    if n <= 3:
        return n >= 2
    if n % 2 == 0 or n % 3 == 0:
        return False
    for i in range(5, int(n ** 0.5) + 1, 6):
        if n % i == 0 or n % (i + 2) == 0:
            return False
    return True
*/
//int integer_test_irr();

#endif


