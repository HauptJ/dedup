//   File: polynomials.h
//   Name: Joshua Haupt, Nick Kirkpatrick, Spencer ???
//   Date: Spring 15
// Course: CS 495 - Crk Group
//   Desc: This file includes the polynomial math function declarations. 
//         
//         
//         
//
//         Other files required: main.c, polynomials.h, bit_array.h, toStringFctns.c, toStringFctns.h


#include "bit_array.h"
//#include "toStringFctns.h" //there just in case


unsigned long createRandomLongPoly();

// irreducible
unsigned long createIrreducible();
int getReducibility(unsigned long);

// poly bit math
bits poly_mult(bits, bits);
int poly_degree(bits);
int poly_hasdegree(bits, int);
bits poly_mod(bits, bits);
bits poly_gcd(bits, bits);
bits poly_modPow(bits base, bits exp, bits mod);
int poly_empty(bits);
bits poly_add(bits, bits);
bits poly_subtract(bits, bits);
bits poly_reduceExp(bits, int);

//report
void print_report();
void to_hex(bits);


