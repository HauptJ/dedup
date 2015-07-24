//   File: createIrreducible.c
//   Name: Joshua Haupt, Nick Kirkpatrick, Spencer ???
//   Date: Spring 15
// Course: CS 495 - Crk Group
//   Desc: This file includes the test cases for createIrreducible() and getIrreducibility() functions.
//         
//         
//         
//
//         Other files required: main.c, polynomials.h, bit_array.h, toStringFctns.c, toStringFctns.h

#include "createIrreducible.h"
#include "toStringFctns.h"
#include "bit_array.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>



int main() {
//testing createReducible
  unsigned long polyLong = createRandomLongPoly(); //createRandomLongPoly() works :D
  printf("PolyLong = ");
  printf("\n%lu\n", polyLong); //output polyLong

//testing createIrreducible
  unsigned long irrPoly = createIrreducible();
  
  char* polyString;
  polyString = toStringPoly(irrPoly); //now testing toStringPoly
  printf("irrPoly = ");
  printf("\n%lu\n", irrPoly); //output irrPoly
  
  
  return 0;
}



// Function: createRandomLongPoly() 
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


// Function: createIrreducible()
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
 while (0) { 
   Polynomial = createRandomLongPoly();
   if (getReducibility(Polynomial))
   printf("\n%lu\n", Polynomial);
     return Polynomial;
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
// Why not return true or false? - of course C was written by the dinosours and booleans did not exist
// during that era. :/
int getReducibilty(unsigned long val) {
  bits temp = bits_initlong(val);
  int degree = poly_degree(temp);
  bits one = bits_initlong(1l);
  int i = 1;

  for(; i <= degree/2; i++) {
    bits b = poly_reduceExp(temp, i);
    bits g = poly_gcd(temp, b);
    if (!eq(one, g))
      return 1;
  }
  return 0;
}


