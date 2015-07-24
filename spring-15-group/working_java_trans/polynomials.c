/*
// Documentation could be better :/
// As in follow the programming style guide
// https://www.cs.siue.edu/programming-style-guide
*/

//   File: polynomials.c
//   Name: Joshua Haupt, Nick Kirkpatrick, Spencer Smith
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




// Function: createRandomLongPoly() 
//
// Desc: Creates a random long 
//       
//       
//
// Pre: none
//      
//      
//
// Post: return unsigned long

bits createRandomLongPoly() {

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
    //printf("\n%lu\n", polyLong );


    bits temp = bits_initlong(polyLong);
    // OR low byte by 0x01 and high byte by 0x80 
    // so the first and last bits are 1
    temp.byte[0] |= 0x80;
    temp.byte[arraySize-1] |= 0x01;
    
    return temp;
}


// Function: createRandomPoly()
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


bits createRandomPoly(int size) {
    if (size > BYTES)
    	size = BYTES;
    	
    unsigned char temp_array[size];
    int i;
    for (i = 0; i < size; i++)
        temp_array[i] = rand()%256;
    	
    // OR low byte by 0x01 and high byte by 0x80 
    // so the first and last bits are 1
    temp_array[0] |= 0x80;
    temp_array[size - 1] |= 0x01;
    //printf("%x \n", temp_array[1]);
    bits temp = bits_initbytes(temp_array, size); 
    //printf("%s \n", toStringHex(temp));
    return temp;
}


// Function: createIrreduciblelong()
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

bits createIrreduciblelong() { 
 // counts number of randomly generated before finding irreducible
 int attempts = 0; 
 
 bits Polynomial;
 while (1) { 
   attempts++;
   Polynomial = createRandomLongPoly();
   if (getReducibility(Polynomial)) {
     printf("Attempts: %d \n", attempts); // for testing
     return Polynomial;
   }
 }
}

// Function: createIrreducible()
//
// Desc: Creates an irreducible polynomial of arbitary size
//       
//       
//
// Pre: size of polynomial
//      
//      
//
// Post: Returns an irreducable polynomial.


bits createIrreducible(int size) { 
 int attempts = 0; 
 bits Polynomial;
 while (1) { 
   attempts++;
   Polynomial = createRandomPoly(size);
   if (getReducibility(Polynomial)) {
     printf("Attempts: %d \n", attempts); // for testing
     return Polynomial;
     }
 }
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

// BenOr algorithm
int getReducibility(bits val) {
  bits temp = bits_initbits(val);
  int degree = poly_degree(temp);
  bits one = bits_initlong(1l);
  bits x = bits_initlong(2l);
  int i;

// trival cases
  if(eq(temp, one))
    return 0;

  if(eq(temp, x))
    return 0;


  for(i = 1; i <= degree/2; i++) {
    bits b = poly_reduceExp(temp, i);
    bits g = poly_gcd(temp, b);
    if (!eq(one, g))
      return 0;
  }
  return 1;
}



/*---Polynomial Bit Functions---*/



// Function: poly_mult //needs fixing
//
// Desc: Multiplies two polynomials in GF(2^K)
//       
//       
//
// Pre: needs two bits polynomials 
//      
//      
//
// Post: return result as new bit polynomial

// poly bit math
bits poly_mult(bits a, bits b) {// needs testing
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
                    deg_b= (b.byte[i_b] & (1 << i_bbit)) ? (BYTES - i_b - 1) * 8 + i_bbit : 0;
                    if ((a.byte[i_a] & (1 << i_abit)) != 0 &&
                    (b.byte[i_b] & (1 << i_bbit)) != 0){
                        sum = deg_a + deg_b ;
                        result.byte[BYTES-1-(sum / 8)] ^= (1 << (sum % 8));
                    }
                }
            }

        }
    }

    return result; //returns null value when value when bytes greater than 32
}



// Function: poly_degree
//
// Desc: finds the highest degree in the bit polynomial 
//       
//       
//
// Pre: must pass a one bit polynomial 
//      
//      
//
// Post: returns degree as an int

int poly_degree(bits a) {
  bits temp = bits_initbits(a);
  int i, j;
  for (i = 0; i < BYTES; i++)
    if (!(temp.byte[i] == 0))
      for (j = 7; j >= 0; j--)
	if ((temp.byte[i] >> j) & 0x01)
	  return (BYTES - i) * 8 - (7 - j) - 1;
  return -1;
}



// Function: poly_hasdegree
//
// Desc: checks if a bit polynomial has
//       a specific degree
//       
// 
// Pre: must pass a bit polynomial and 
//      degree
//      
//
// Post: return 1 for true and 0 for false

int poly_hasdegree(bits a, int degree) {
  bits temp = bits_initbits(a);
  int byte_offset = BYTES - (degree)/8 - 1;
  int bit_offset = degree%8;
  if ((temp.byte[byte_offset] >> bit_offset) & 0x01)
    return 1;
  return 0;
}



// Function: poly_mod
//
// Desc: finds a mod b in GF(2^k)
//       
//       
//
// Pre: must pass two bit polynomials 
//      
//      
//
// Post: returns result as new bit polynomial

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



// Function: poly_gcd
//
// Desc: finds gcd of two bit polynomials in GF(2^k)
//       using extended euclidean
//       
//
// Pre: must pass two bit polynomials
//      
//      
//
// Post: returns result as new bit polynomials

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



// Function: poly_modPow
//
// Desc: Finds (base) ^ (exp) mod (mod) in
//       GF(2^k)
//       
//
// Pre: must pass three bit polynomials 
//      
//      
//
// Post: returns result as bit polynomial

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


// Function: poly_add
//
// Desc: performs (a + b) in GF(2^k) 
//       
//       
//
// Pre: must pass two bit polynomials 
//      
//      
//
// Post: returns result as bit polynomial


bits poly_add(bits a, bits b) {
  bits temp = xor(a, b);
  return temp;
}

// Function: poly_subtract
//
// Desc: performs (a - b) in GF(2^k) 
//       
//       
//
// Pre: must pass two bit polynomials 
//      
//      
//
// Post: returns result as bit polynomial

bits poly_subtract(bits a, bits b) {
  bits temp = xor(a, b);
  return temp;
}



// Function: poly_empty
//
// Desc: test if a polynomial is empty
//       
//       
//
// Pre: must pass one bit polynomial
//      
//      
//
// Post: return 1 for true and 0 for false

int poly_empty(bits a) {
  int i;
  for (i = 0; i < BYTES; i++)
    if (a.byte[i] != 0)
      return 0;
  return 1;
}



// Function: poly_reduceExp
//
// Desc: finds (x ^ (2 ^ p) - x) mod (a) in GF(2^k)
//       
//       
//
// Pre: must pass one bit polynomial and int p 
//      representing the power of 2 that x will
//      be raised to
//
// Post: return result as bit polynomial

bits poly_reduceExp(bits a, int p) {
  bits temp = bits_initbits(a);
  bits X = bits_initlong(2l);
  
  bits exp = bits_initlong(1l);
  exp = shiftl(exp, p);

  bits result = bits_initbits(poly_modPow(X, exp, temp));

  return xor(result, (poly_mod(X, temp)));
}


// Function: poly_print_report
//
// Desc: tests all poly math functions and prints
//       report
//       
//
// Pre: none
//      
//      
//
// Post: none

void poly_print_report() {
  printf("----------\npoly tests\n----------\n");
  //multiply
  
  printf("muliply: ");
  (eq(poly_mult(bits_initlong(87l), bits_initlong(131l)), 
     bits_initlong(11129l)))?printf("ok\n"):printf("failed\n");

  printf("muliply: ");
  (eq(poly_mult(bits_initlong(0x57), bits_initlong(0x83)), 
      bits_initlong(0x2B79)))?printf("ok\n"):printf("failed\n");
      
  printf("muliply: ");
  (eq(poly_mult(bits_initlong(0xD6), bits_initlong(0x29)), 
      bits_initlong(0x1CA6)))?printf("ok\n"):printf("failed\n");

  //degree - fails if bytes > 32 fixed!
  printf("degree: ");
  (poly_degree(bits_initlong(256)) == 8)?printf("ok\n"):printf("failed\n");
  
  printf("degree: ");
  (poly_degree(bits_initlong(0xFF00FF00)) == 31)?printf("ok\n"):printf("failed\n");
  

  //hasdegree
   printf("hasdegree: ");
   (poly_hasdegree(bits_initlong(257), 0))?printf("ok\n"):printf("failed\n");
   
   printf("hasdegree: ");
   (poly_hasdegree(bits_initlong(512), 9))?printf("ok\n"):printf("failed\n");
   

  //mod
   printf("mod: ");
   (eq(poly_mod(bits_initlong(16254l), bits_initlong(283l)), 
       bits_initlong(1l)))?printf("ok\n"):printf("failed\n");
       
   printf("mod: ");
   (eq(poly_mod(bits_initlong(0xAC), bits_initlong(0x45)), 
       bits_initlong(0x26)))?printf("ok\n"):printf("failed\n");
       
   printf("mod: ");
   (eq(poly_mod(bits_initlong(0x248), bits_initlong(0xC3)), 
       bits_initlong(0x1)))?printf("ok\n"):printf("failed\n");
           
  //gcd
   printf("gcd: ");
   (eq(poly_gcd(bits_initlong(16254l), bits_initlong(283l)), 
       bits_initlong(1l)))?printf("ok\n"):printf("failed\n");
       
   printf("gcd: ");
   (eq(poly_gcd(bits_initlong(125l), bits_initlong(25l)), 
       bits_initlong(25l)))?printf("ok\n"):printf("failed\n");
       
   printf("gcd: ");
   (eq(poly_gcd(bits_initlong(0x1D6), bits_initlong(0x2D)), 
       bits_initlong(0x5)))?printf("ok\n"):printf("failed\n");
       

  //modPow
   printf("modPow: ");
   (eq(poly_modPow(bits_initlong(0x6), bits_initlong(0x3), 
		   bits_initlong(0xb)), bits_initlong(0x7)))
     ?printf("ok\n"):printf("failed\n");
     
   printf("modPow: ");
   (eq(poly_modPow(bits_initlong(0x53), bits_initlong(0x5), 
		   bits_initlong(0xD)), bits_initlong(0x3)))
     ?printf("ok\n"):printf("failed\n");
     

  //empty
   printf("empty: ");
   (poly_empty(bits_init()))?printf("ok\n"):printf("failed\n");
   
   printf("empty: ");
   (poly_empty(bits_initones()) == 0)?printf("ok\n"):printf("failed\n");

  //add
   printf("add: ");
   (eq(poly_add(bits_initlong(349525), bits_initlong(699050)), 
       bits_initlong(0xFFFFF)))?printf("ok\n"):printf("failed\n");
   
  //subtract
   printf("subtract: ");
   (eq(poly_subtract(bits_initlong(349525), bits_initlong(699050)), 
       bits_initlong(0xFFFFF)))?printf("ok\n"):printf("failed\n");
  
  //reduceExp
   printf("reduceExp: ");
   (eq(poly_reduceExp(bits_initlong(0xc), 5), 
       bits_initlong(0x6)))?printf("ok\n"):printf("failed\n");
 
 printf("reduceExp: ");
   (eq(poly_reduceExp(bits_initlong(0x253), 12), 
       bits_initlong(0x64)))?printf("ok\n"):printf("failed\n");
       

}

void irreducible_print_report() {
   printf("\n----------\nirreducible tests\n----------\n");
  //irreducible 
  
  // Generate 10 irreducible 64 bit polynomails
  printf("Generating Random Polynomials... \n");
  int i, j;
  srand(time(0));
  for (i = 1; i < 9; i++) {
     bits test = createIrreducible(i*8); 
     printf("%s \n", toStringHex(test));
    for (j = 0; j < 1; j++) {
       printf("Irreducible: %d ", getReducibility(test));
     }
     printf("\n\n");
   }
  
   
   // test known irreducible values       
   printf("Testing Known Irreducibles.. \n");                                                              
   printf("3915039707: %d \n", getReducibility(bits_initlong(3915039707))); //from crk
   // found on wolfram
   printf("0x7: %d \n", getReducibility(bits_initlong(0x7)));
   printf("0xb: %d \n", getReducibility(bits_initlong(0xb)));
   printf("0xd: %d \n", getReducibility(bits_initlong(0xd)));
   printf("0x13: %d \n", getReducibility(bits_initlong(0x13)));
   printf("0x1f: %d \n", getReducibility(bits_initlong(0x1f)));
   printf("0x19: %d \n", getReducibility(bits_initlong(0x19)));
   printf("0x25: %d \n", getReducibility(bits_initlong(0x25)));
   printf("0x2f: %d \n", getReducibility(bits_initlong(0x2f)));
   printf("0x29: %d \n", getReducibility(bits_initlong(0x29)));
   printf("0x3b: %d \n", getReducibility(bits_initlong(0x3b)));
   printf("0x3d: %d \n", getReducibility(bits_initlong(0x3d)));
   printf("0x37: %d \n", getReducibility(bits_initlong(0x37)));
}





