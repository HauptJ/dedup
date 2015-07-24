#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "bit_array.h"

// bit initializations                                                    
bits bits_init() {
  bits temp;
  memset((unsigned char*)temp.byte, 0, BYTES); 
  return temp;
}
bits bits_initones() {
  bits temp = bits_init();
  memset(temp.byte, 255, BYTES);
  return temp;
}
bits bits_initlong(unsigned long int val) {
  bits temp = bits_init();
  int size = sizeof(unsigned long int);
  unsigned int temp_int = 0;
  /*
  if(endianness() == LITTLE) {
    // swap last 32 bits
    temp_int |= swapbytes((unsigned int)val);
    // swap first 32 bits and swap first and last
    val = swapbytes((unsigned int)(val >> 32)) | (((unsigned long int)temp_int) << 32);
  }   
  */  
  if(endianness() == LITTLE) {
    int i;
    unsigned long bytes = 0, templ = 0, tempr = 0;
    for(i = 0; i < size/2; i++) {
      templ &= 0;
      tempr &= 0;
      templ |= (val >> (i * 8));
      templ = (templ << ((size-i-1)*8));
      tempr |= (val >> ((size-i-1)*8));
      
      bytes |= templ;
      bytes |= tempr;
      
    }
    // if size is odd transfers middle byte
    if ((size/2)%2){
      templ = 0;
      templ |= (val >> (size/2*8));
      templ = templ << (size/2*8);
      bytes |= templ;
    }
    val = bytes;
  }
  
  if (BYTES < size) {
    size -= BYTES;
    val = val << (size*8);
    // pointer to lowest memory
    memcpy((unsigned char*)(&(temp.byte[0])), 
	   (unsigned char*)(&val), BYTES);
  } else {
    memcpy((unsigned char*)(&(temp.byte[BYTES-size])),
	   (unsigned char*)(&val), size);
  }

  return temp;
}

unsigned int swapbytes(unsigned int val) {
  // swap middle bytes
  val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
  // swap outside bytes
  return (val << 16) | (val >> 16);
}

bits bits_initbits(bits a) {
  bits temp = bits_init();
  memcpy(temp.byte, a.byte, BYTES);
  return temp;
}

int endianness(){
  /*
    little-endian
    higher memory
    ----->
    +----+----+----+----+
    |0x01|0x00|0x00|0x00|
    +----+----+----+----+
    A
    |
    &x
    
    big-endian
    +----+----+----+----+
    |0x00|0x00|0x00|0x01|
    +----+----+----+----+
    A
    |
    &x
  */
  int i = 1;
  if ((char*)&i)
    return LITTLE;
  return BIG; //little on mac
}

// bit comparisons                                                        
int lte(bits a, bits b) {
  int i;
  if (eq(a, b))
    return 1;
  for (i = 0; i < BYTES; i++){
    if(a.byte[i] < b.byte[i])
      return 1;
    else
      return 0;
  }
  return 0;
}

int gte(bits a, bits b) {
  int i;
  if (eq(a, b))
    return 1;
  for (i = 0; i < BYTES; i++){
    if(a.byte[i] > b.byte[i])
      return 1;
    else
      return 0;
  }
  return 0;
}
int gt(bits a, bits b) {
  int i;
  if (eq(a, b))
    return 0;
  for (i = 0; i < BYTES; i++){
    if(a.byte[i] > b.byte[i])
      return 1;
    else
      return 0;
  }
  return 0;
}
int lt(bits a, bits b) {
  int i;
  if (eq(a, b))
    return 0;
  for (i = 0; i < BYTES; i++){
    if(a.byte[i] < b.byte[i])
      return 1;
    else
      return 0;
  }
  return 0;
}
int eq(bits a, bits b) {
  int i;
  for (i = 0; i < BYTES; i++)
    if (a.byte[i] != b.byte[i])
      return 0;
  return 1;
}

// bit ops                                                                
bits and(bits a, bits b) {
  int i;
  bits temp;
  for (i = 0; i < BYTES; i++) 
    temp.byte[i] = a.byte[i] & b.byte[i];
  return temp;
}
bits or(bits a, bits b) {
  int i;
  bits temp;
  for (i = 0; i < BYTES; i++) 
    temp.byte[i] = a.byte[i] | b.byte[i];
  return temp;
}

bits xor(bits a, bits b) {
  int i;
  bits temp;
  for (i = 0; i < BYTES; i++) 
    temp.byte[i] = a.byte[i] ^ b.byte[i];
  return temp;
}

bits shiftl(bits a, int times) {
  int j;
  bits temp = bits_initbits(a);
  for (j = 0; j < times; j++) {
    unsigned char firstbit = 0;
    unsigned char byte = 0;
    int i = BYTES -1;
    for (; i >= 0; i--) {
      /* Starting from right shift bit left and
	 OR with bitmask first bit of byte of 
	 previous byte. Last bit in first byte is 
	 always 1'b0
       */
      byte = (temp.byte[i] << 1) | firstbit;
      firstbit = temp.byte[i] >> 7;
      temp.byte[i] = byte; 
    }
  }
  return temp;
}

bits shiftr(bits a, int times) {
  int j, i;
  bits temp = bits_initbits(a);
  for (j = 0; j < times; j++) {
    unsigned char lastbit = 0;
    unsigned char byte = 0;
    for (i = 0; i < BYTES; i++) {
      /* Starting from left shift bit right and
	 OR with bitmask last bit of byte of 
	 previous byte. First bit in first byte is 
	 always 1'b0
       */
      byte = (temp.byte[i] >> 1) | lastbit;
      lastbit = (temp.byte[i]) << 7;
      temp.byte[i] = byte;
     }
  }
  return temp;
}

void print_hex(bits a) {
  int i;
  for(i = 0; i < BYTES; i++)
    printf("0x%x ", a.byte[i]);

  printf("\n");
}
