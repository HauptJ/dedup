//   File: createIrreducible.h
//   Name: Joshua Haupt, Nick Kirkpatrick, Spencer ???
//   Date: Spring 15
// Course: CS 495 - Crk Group
//   Desc: Testing createIrreducible()
//         
//         
//         
//
//         Other files required: createIrreducible.c

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "bit_array.h"

unsigned long createRandomLongPoly();

// irreducible
unsigned long createIrreducible();
int getReducibility(unsigned long);
