//   File: polynomials.c
//   Name: Joshua Haupt, Nick Kirkpatrick, Spencer ???
//   Date: Spring 15
// Course: CS 495 - Crk Group
//   Desc: This file includes the polynomial math functions. 
//         
//         
//         
//
//         Other files required: main.c, polynomials.h, bit_array.h, toStringFctns.c, toStringFctns.h



#include "polynomials.h"
#include "bit_array.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>



// Function: print_report() 
//
// Desc: prints output of test cases
//       
//       
//
// Pre: none
//      
//      
//
// Post: prints output

void print_report() {
  //createRandomLongPoly() 
  printf("Testing createRandomLongPoly() - now in fctn\n");
  unsigned long polyLong = createRandomLongPoly(); //createRandomLongPoly() works :D
  printf("PolyLong = \n");
  printf("\n%lu\n", polyLong); //output polyLong

  printf("Testing getReducibility() - now in fctn\n");
  unsigned long testIrr = 6531160855165088358;
  int irrResult = getReducibility(testIrr);
  printf("\n%lu\n", irrResult);
  

  printf("Testing createIrreducible() - now in fctn\n");
  unsigned long irrPoly = createIrreducible();
  printf("irrPoly = \n");
  printf("\n%lu\n", irrPoly); //output polyLong

}


// Function: createRandomLongPoly() - works
//
// Desc: 
//       
//       
//
// Pre: 
//      
//      
//
// Post: 

unsigned long createRandomLongPoly() {

    const int arraySize=8;
    unsigned char randomNum;
    unsigned char polyTempArr[arraySize];

    // populate array
    int i=0;
    for (; i < arraySize; i++)
    {
     	randomNum= rand()%256;
        polyTempArr[i]= randomNum;
    }
     //convert the array to a long
    unsigned long polyLong;
    memcpy (&polyLong, polyTempArr, sizeof (long));
    printf("\n%lu\n", polyLong );

    return polyLong;
}



// Function: createIrreducible() - works if getReducibility() works
//
// Desc: Creates an irreducible polynomial
//       
//       
//
// Pre: None
//      
//      
//
// Post: Returns an irreducable polynomial.

unsigned long createIrreducible() { 
 unsigned long Polynomial;
 while (1) { 
   Polynomial = createRandomLongPoly();
   if (getReducibility(Polynomial))
     return Polynomial;
 }
}



// Function: getReducibility() - testing
//
// Desc: 
//       
//       
//
// Pre: 
//      
//      
//
// Post: 

// BenOr algorithm
int getReducibility(unsigned long val) {
  bits temp = bits_initlong(val);
  int degree = poly_degree(temp);
  bits one = bits_initlong(1l);
  bits x = bits_initlong(2l);
  int i;


  if(eq(temp, one))
    return 0;

  if(eq(temp, x))
    return 0;


  for(i = 1; i <= degree/2; i++) {
    bits b = poly_reduceExp(temp, i);
    bits g = poly_gcd(temp, b);
    if (!eq(one, g))
      return 1;
  }
  return 0;
}



/*---Polynomial Bit Functions---*/



// Function: 
//
// Desc: 
//       
//       
//
// Pre: 
//      
//      
//
// Post: 

// poly bit math
bits poly_mult(bits a, bits b) {// needa testing
    int deg_a = 0;
    int deg_b = 0;
    int sum = 0;
    bits result =  bits_init();
    int i_a, i_b, i_abit, i_bbit;
    for (i_a = 0; i_a < BYTES; i_a++){
        for (i_b = 0; i_b < BYTES; i_b++){

            for (i_abit = 0; i_abit < 8; i_abit++){
                for (i_bbit = 0; i_bbit < 8; i_bbit++){

                    deg_a = (a.byte[i_a] & (1 << i_abit)) ? (BYTES - i_a - 1) * 8 + i_abit : 0;
                    deg_b = (b.byte[i_b] & (1 << i_bbit)) ? (BYTES - i_b - 1) * 8 + i_bbit : 0;
                    if ((a.byte[i_a] & (1 << i_abit)) != 0 &&
                    (b.byte[i_b] & (1 << i_bbit)) != 0){
                        sum = deg_a + deg_b ;
                        result.byte[BYTES-1-(sum / 8)] ^= (1 << (sum % 8));
                    }
                }
            }

        }
    }

    return result;
}



// Function: 
//
// Desc: 
//       
//       
//
// Pre: 
//      
//      
//
// Post: 

int poly_degree(bits a) {
  bits temp = bits_initbits(a);
  int i;
  int j = BYTES -1;
  for (i = 0; i < BYTES; i++)
    if (!(temp.byte[i] == 0))
      for (; j >= 0; j--)
	if ((temp.byte[i] >> j) & 0x01)
	  return (BYTES - i) * BYTES - (7 - j) - 1;
  return 0;
}



// Function: 
//
// Desc: 
//       
//       
//
// Pre: 
//      
//      
//
// Post: 

int poly_hasdegree(bits a, int degree) {
  bits temp = bits_initbits(a);
  int byte_offset = BYTES - (degree)/8 - 1;
  int bit_offset = degree%8;
  if ((temp.byte[byte_offset] >> bit_offset) & 0x01)
    return 1;
  return 0;
}



// Function: 
//
// Desc: 
//       
//       
//
// Pre: 
//      
//      
//
// Post: 

bits poly_mod(bits a, bits b) {
  bits temp = bits_initbits(a);
  bits shifted = bits_init();
  int da = poly_degree(a);
  int db = poly_degree(b);
  int i;

  for (i = (da - db); i >= 0; i--) {
    if (poly_hasdegree(temp, (i + db))) {
      shifted = shiftl(b, i);
      temp = xor(temp, shifted);
    }
  }

  return temp;
}



// Function: 
//
// Desc: 
//       
//       
//
// Pre: 
//      
//      
//
// Post: 

bits poly_gcd(bits a, bits b) {
  bits temp1 = bits_initbits(a);
  bits temp2 = bits_initbits(b);

  while (!poly_empty(temp2)) {
     bits temp3 = bits_initbits(temp2);
     temp2 = poly_mod(temp1, temp2);
     temp1 = temp3;
  }
  return temp1;
}



// Function: 
//
// Desc: 
//       
//       
//
// Pre: 
//      
//      
//
// Post: 

bits poly_modPow(bits base, bits exp, bits mod) {
   bits result = bits_initlong(1l);
  bits b = bits_initbits(base);
  bits m = bits_initbits(mod);
  
  while (!poly_empty(exp)) {
    if (exp.byte[BYTES-1] & 0x0001) {
      result = poly_mod(poly_mult(result, b), m);
    }
    exp = shiftr(exp, 1);
    b = poly_mod(poly_mult(b, b), m);
  }
  return result;
}


// Function: 
//
// Desc: 
//       
//       
//
// Pre: 
//      
//      
//
// Post: 


bits poly_add(bits a, bits b) {
  bits temp = xor(a, b);
  return temp;
}

// Function: 
//
// Desc: 
//       
//       
//
// Pre: 
//      
//      
//
// Post: 

bits poly_subtract(bits a, bits b) {
  bits temp = xor(a, b);
  return temp;
}



// Function: 
//
// Desc: 
//       
//       
//
// Pre: 
//      
//      
//
// Post: 

int poly_empty(bits a) {
  int i;
  for (i = 0; i < BYTES; i++)
    if (a.byte[i] != 0)
      return 0;
  return 1;
}



// Function: 
//
// Desc: 
//       
//       
//
// Pre: 
//      
//      
//
// Post: 

bits poly_reduceExp(bits a, int p) {
  bits temp = bits_initbits(a);
  bits X = bits_initlong(2l);
  
  // find X^p
  unsigned long num = 2; 
  int i = p;
  for (; i > 0; i--)
    num *=2;
  bits power = bits_initlong((num));

  bits result = bits_initbits(poly_modPow(X, power, temp));

  return xor(result, (poly_mod(X, temp)));
}

void to_hex(bits a) {
  int i;
  for (i = 0; i < BYTES; i++) 
    printf("%x", a.byte[i]);
  printf("\n");
}

/*void print_report() {
  //multiply
  printf("muliply: ");
  (eq(poly_mult(bits_initlong(87l), bits_initlong(131l)), 
     bits_initlong(11129l)))?printf("ok\n"):printf("failed\n");

  printf("muliply: ");
  (eq(poly_mult(bits_initlong(0x57), bits_initlong(0x57)), 
      bits_initlong(0x1115)))?printf("ok\n"):printf("failed\n");

  //degree
  printf("degree: ");
  (poly_degree(bits_initlong(256)) == 8)?printf("ok\n"):printf("failed\n");

  //hasdegree
   printf("hasdegree: ");
   (poly_hasdegree(bits_initlong(257), 0))?printf("ok\n"):printf("failed\n");

  //mod
   printf("mod: ");
   (eq(poly_mod(bits_initlong(16254l), bits_initlong(283l)), 
       bits_initlong(1l)))?printf("ok\n"):printf("failed\n");
   
   
  //gcd
   printf("gcd: ");
   (eq(poly_gcd(bits_initlong(16254l), bits_initlong(283l)), 
       bits_initlong(1l)))?printf("ok\n"):printf("failed\n");

  //modPow
   printf("modPow: ");
   (eq(poly_modPow(bits_initlong(0x57), bits_initlong(0x83), 
		   bits_initlong(0x11B)), bits_initlong(0xC1)))
     ?printf("ok\n"):printf("failed\n");


  //empty
   printf("empty: ");
   (poly_empty(bits_init()))?printf("ok\n"):printf("failed\n");

  //add
   printf("add: ");
   (eq(poly_add(bits_initlong(349525), bits_initlong(699050)), 
       bits_initlong(0xFFFFF)))?printf("ok\n"):printf("failed\n");
   
  //subtract
   printf("subtract: ");
   (eq(poly_subtract(bits_initlong(349525), bits_initlong(699050)), 
       bits_initlong(0xFFFFF)))?printf("ok\n"):printf("failed\n");
  
  //reduceExp

  //irreducible 
}

*/



