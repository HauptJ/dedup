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

bits bits_initbytes(char array[], int size) {
  bits temp = bits_init();
  int i;
  for (i = 1; i <= size; i++)
    temp.byte[BYTES-i] = array[size-i];
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
      templ |= (((val >> (i * 8)) << ((size-1)*8)) >> (i * 8));
      tempr |= (((val << (i * 8)) >> ((size-1)*8)) << (i * 8));
      //printf("left: %lx right: %lx \n", templ, tempr);
      
      bytes |= templ;
      bytes |= tempr;
      //printf("byte: %lx \n", bytes);
    }
    // if size is odd transfers middle byte
    if ((size)%2){
      templ = 0;
      templ |= (((val >> (size/2*8)) << ((size - 1) *8)) << (size/2*8));
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




// bit comparisons - NO SHIT tell what they are
//less than equall to comparison                                                       
int lte(bits a, bits b){
  int i;
  if (eq(a,b))
    return 1;
  for (i = 0; i < BYTES; i++){
    if (!(a.byte[i] == 0 && b.byte[i] == 0)){
      if (a.byte[i] < b.byte[i])
	return 1;
      else
	return 0;
    }
  }
  return 1;
}



//greater than or equal to comparison
int gte(bits a, bits b){
  int i;
  if (eq(a,b))
    return 1;
  for (i = 0; i < BYTES; i++){
    if (!(a.byte[i] == 0 && b.byte[i] == 0)){
      if (a.byte[i] > b.byte[i])
	return 1;
      else
	return 0;
    }
  }
  return 1;
}



//greater than comparison
int gt(bits a, bits b){
  int i;
  for (i = 0; i < BYTES; i++){
    if (!(a.byte[i] == 0 && b.byte[i] == 0)){
      if (a.byte[i] > b.byte[i])
	return 1;
      else
	return 0;
    }
  }
  return 0;
}



//less than comparison
int lt(bits a, bits b){
  int i;
  for (i = 0; i < BYTES; i++){
    if (!(a.byte[i] == 0 && b.byte[i] == 0)){
      if (a.byte[i] < b.byte[i])
	return 1;
      else
	return 0;
    }
    
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




void bit_array_print_report() {
    printf("----------\nbit_array tests\n----------\n");
    //---------p
    //inits
    //----------

        //bits_init
    printf("bits_init: ");
    1==2?printf("ok\n"):printf("failed\n");

    //bits_initones
    printf("bits_initones: ");
    1==2?printf("ok\n"):printf("failed\n");

    //bits_initlong
    printf("bits_initlong: ");
    1==2?printf("ok\n"):printf("failed\n");

    //bits_initbits
    printf("bits_initbits: ");
    1==2?printf("ok\n"):printf("failed\n");

    //endianness - need big endian device to test
    printf("endianness: ");
    1==2?printf("ok\n"):printf("failed\n");

    //----------
    //comparisons $$$$$$$$$$$$$$MAY NEED WORK$$$$$$ - NO SHIT
    //----------

    //lte - Less than equal to
    printf("lte: ");
    1==lte(bits_initlong(555436),bits_initlong(555436))&&1==lte(bits_initlong(8),bits_initlong(9))&&0==lte(bits_initlong(100056658),bits_initlong(1000))?printf("ok\n"):printf("failed\n");

    //gte - Greater than equal to
    printf("gte: ");
    1==gte(bits_initlong(555436),bits_initlong(555436))&&0==gte(bits_initlong(8),bits_initlong(9))&&1==gte(bits_initlong(100056658),bits_initlong(1000))?printf("ok\n"):printf("failed\n");

    //gt - Greater than
    printf("gt: ");
    0==gt(bits_initlong(555436),bits_initlong(555436))&&0==gt(bits_initlong(8),bits_initlong(9))&&1==gt(bits_initlong(100056658),bits_initlong(1000))?printf("ok\n"):printf("failed\n");

    //lt - Less than
    printf("lt: ");
    0==lt(bits_initlong(555436),bits_initlong(555436))&&1==lt(bits_initlong(8),bits_initlong(9))&&0==lt(bits_initlong(100056658),bits_initlong(1000))?printf("ok\n"):printf("failed\n");

    //eq - Equal to
    printf("eq: ");
    (1==eq(bits_initlong(555436),bits_initlong(0x879AC))&&0==eq(bits_initlong(100056798),bits_initlong(0x5F7DE))?printf("ok\n"):printf("failed\n"));

    //----------
    //ops
    //----------


    //and
    printf("and: ");
    eq(bits_initlong(0x000b030a28242002),and(bits_initlong(4701532727728676918),bits_initlong(453609631608285195)))&&!eq(bits_initlong(0x000b030a28242002),and(bits_initlong(4701532727728676918),bits_initlong(453609631608222222)))?printf("ok\n"):printf("failed\n");
    

    //or
    printf("or: ");
    eq(bits_initlong(0x477fbb9f7eb5283f),or(bits_initlong(4701532727728676918),bits_initlong(453609631608285195)))&&!eq(bits_initlong(0x477fbb9f7eb5283f),or(bits_initlong(4701532727728676918),bits_initlong(453609631608285206)))?printf("ok\n"):printf("failed\n");

    //xor
    printf("xor: ");
    eq(bits_initlong(0x4774b8955691083d),xor(bits_initlong(4701532727728676918),bits_initlong(453609631608285195)))&&!eq(bits_initlong(0x477fbb9f7eb5283f),xor(bits_initlong(4701532727728676918),bits_initlong(453609631608285206)))?printf("ok\n"):printf("failed\n");

    //shiftl - shift left
    printf("shiftl: ");
    eq(bits_initlong(0x3f331b2824203600),shiftl(bits_initlong(0x03f331b282420360),4))&&!eq(bits_initlong(0x477fbb9f7eb5283f),shiftl(bits_initlong(4701532727728676918),4))?printf("ok\n"):printf("failed\n");

    //shiftr - shift right
    printf("shiftr: ");
    eq(bits_initlong(0x413f331b2824203),shiftr(bits_initlong(4701532727728676918),4))&&!eq(bits_initlong(0x477fbb9f7eb5283f),shiftr(bits_initlong(4701532727728676918),4))?printf("ok\n"):printf("failed\n");
												  
}
