#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <semaphore.h>
#include "bit_array.h"
  
// bit initializations  

/*!  Function: bits_init 

 Desc: creates bits type with all bytes 
       set to '0000'
       

 Pre: must pass size of bits polynomial in bytes
      ex 8 bytes = 64 bits 
      

 Post: return a new bit polynomial
*/                                                  
bits* bits_init(int size) {
  bits* temp = malloc(sizeof(bits));
  temp->size = size;
  temp->byte = malloc(sizeof(char)*size);
  memset((unsigned char*)temp->byte, 0, size); 
  return temp;
}

/*! Function: bits_initbytes

 Desc: creates bits type from char array
       
       

 Pre: must pass char array and size of array
      
      

 Post: return a new bit polynomial
 */

bits* bits_initbytes(char array[], int size) {
  bits* temp = bits_init(size);
  int i;
  for (i = 1; i <= size; i++)
    temp->byte[temp->size-i] = array[size-i];
  return temp;
}

/*! Function: bits_initlong

 Desc: creates bits type from int or long
       max size is 64 bits since long is 8 bytes
       mostly used for testing 

 Pre: must pass int or long
      
      

 Post: return a new bit polynomial
*/
bits* bits_initlong(unsigned long int val) {
  bits* temp = bits_init(8);
  int size = sizeof(unsigned long int);
  unsigned int temp_int = 0;
  
  if(endianness() == LITTLE) {
    int i;
    unsigned long bytes = 0, templ = 0, tempr = 0;
    for(i = 0; i < size/2; i++) {
      templ &= 0;
      tempr &= 0;
      templ |= (((val >> (i * 8)) << ((size-1)*8)) >> (i * 8));
      tempr |= (((val << (i * 8)) >> ((size-1)*8)) << (i * 8));
      
      bytes |= templ;
      bytes |= tempr;
    }
    // if size is odd transfers middle byte
    if ((size)%2){
      templ = 0;
      templ |= (((val >> (size/2*8)) << ((size - 1) *8)) << (size/2*8));
      bytes |= templ;
    }
    val = bytes;
  }


  if (temp->size < size) {
    size -= temp->size;
    val = val << (size*8);
    // pointer to lowest memory
    memcpy((unsigned char*)(&(temp->byte[0])), 
	   (unsigned char*)(&val), temp->size);
  } else {
    memcpy((unsigned char*)(&(temp->byte[temp->size-size])),
	   (unsigned char*)(&val), size);
  }
  return temp;
}

/*! Function: bits_initbits

 Desc: copy constructor for bits object
       
       

 Pre: must pass bits type and size of new bits 
      polynomial. If size is less then the pass in
      bits type, the size is set to the passed bits 
      type.

 Post: return a new bit polynomial
*/
bits* bits_initbits(bits* a, int size) {
  int i;
  
  // checks if size is less than a->size
  if (a->size > size)
    size = a->size;
  
  bits* temp = bits_init(size);
  memcpy((unsigned char*)(&(temp->byte[temp->size-a->size])),
	   (unsigned char*)(&(a->byte[0])), a->size);

  return temp;
}

/*! Function: bits_initstring

 Desc: creates bits type from hex string
       
       

 Pre: must pass char hex string
      
      

 Post: return a new bit polynomial
*/
bits* bits_initstring(char* hex) {
  int i;
  
  int length = strlen(hex); //2 hex = 1 byte
  int size = length;
  if (length%2)
    size++;
  size /=2;
  
  bits* temp = bits_init(size);

  unsigned int byte = 0;
  if (length%2) {
    sscanf(&hex[0], "%1x", &byte);
    temp->byte[0] = (unsigned char)byte;
    for (i = 0; i < temp->size-1; i++) {
      sscanf(&hex[i*2+1], "%2x", &byte);
      temp->byte[i+1] = (unsigned char)byte;
    }
  }
  else
  { 
    for (i = 0; i < temp->size; i++) {
      sscanf(&hex[i*2], "%2x", &byte);
      temp->byte[i] = (unsigned char)byte;
    }
  }
  return temp;
}
/*!
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
int endianness(){
  
  int i = 1;
  if ((char*)&i)
    return LITTLE;
  return BIG; //little on mac
}



// bit comparisons

//less than equal to comparison  

/*! Function: lte

bits type is less than equal to 
 bits type. can handle bits of any
       size

 Pre: must pass 2 bits type
      
      

 Post: return 0 if a>b or 1 if a<=b
*/                                                     
int lte(bits* a, bits* b){
  int i;
  if (eq(a,b))
    return 1;
    
  // case 1: a->size = b->size
  if (a->size == b->size) {
    for (i = 0; i < a->size; i++){
      if (!(a->byte[i] == 0 && b->byte[i] == 0)){
        if (a->byte[i] < b->byte[i])
	  return 1;
        else
	  return 0;
      }
    }  
    return 1;
  
  } else { 
    // case 2: a->size < b->size
    if (a->size < b->size) {
      int offset = b->size - a->size; 
      for (i = 0; i < b->size; i++) {
        if (i < offset) {
          if (!(b->byte[i] == 0))
            return 0;
        } else {
          if (!(a->byte[i-offset] == 0 && b->byte[i] == 0)){
            if (a->byte[i-offset] < b->byte[i])
	      return 1;
            else
	      return 0;
	    }
	  }
	}
	return 1;
    } else { 
    // case 2: a->size > b->size
    int offset = a->size - b->size;
      for (i = 0; i < a->size; i++) {
        if (i < offset) {
          if (!(a->byte[i] == 0))
            return 1;
        } else {
          if (!(a->byte[i] == 0 && b->byte[i-offset] == 0)){
            if (a->byte[i] < b->byte[i-offset])
	      return 1;
            else
	      return 0;
            }
	  }
        }
        return 1;
    }
  }  
}

//greater than or equal to comparison

/*! Function: gte

 Desc: check if bits type is greater than equal to 
       another bits type. can handle bits of any
       size

 Pre: must pass 2 bits type
      
      

 Post: return 0 if a<b or 1 if a>=b
*/
int gte(bits* a, bits* b){
  int i;
  if (eq(a,b))
      return 1;
  if (a->size == b->size) {
    for (i = 0; i < a->size; i++){
      if (!(a->byte[i] == 0 && b->byte[i] == 0)){
        if (a->byte[i] > b->byte[i])
	  return 1;
        else
	  return 0;
    }
  }
  return 1;
  
  } else { 
    //a < b
    int offset = (b->size - a->size);
    if (a->size < b->size) { 
      for (i = 0; i < b->size; i++) {
        if (i < offset) {
          if (!(b->byte[i] == 0))
            return 0;
        } else {
          if (!(a->byte[i-offset] == 0 && b->byte[i] == 0)){
            if (a->byte[i-offset] > b->byte[i])
	      return 1;
            else
	      return 0;
	    }
	  }
        }
        return 1;
    } else { 
      int offset = a->size - b->size;
      for (i = 0; i < a->size; i++) {
        if (i < offset) {
          if (!(a->byte[i] == 0))
            return 1;
        } else {
          if (!(a->byte[i] == 0 && b->byte[i-offset] == 0)){
            if (a->byte[i] > b->byte[i-offset])
	      return 1;
            else
	      return 0;
          }
        }
      }
      return 1; 
    }
  }
}

/*! Function: gt

 Desc: check if bits type is greater than 
       another bits type. can handle bits of any
       size

 Pre: must pass 2 bits type
      
      

 Post: return 0 if a<=b or 1 if a>b
*/
int gt(bits* a, bits* b){
  int i;
  if (eq(a,b))
    return 0;
  if (a->size == b->size) {
    for (i = 0; i < a->size; i++){
      if (!(a->byte[i] == 0 && b->byte[i] == 0)){
        if (a->byte[i] > b->byte[i])
	  return 1;
        else
	  return 0;
    }
  }
  return 1;
  } else { 
    //a < b
    int offset = (b->size - a->size);
    if (a->size < b->size) { 
      for (i = 0; i < b->size; i++) {
        if (i < offset) {
          if (!(b->byte[i] == 0))
            return 0;
        } else {
          if (!(a->byte[i-offset] == 0 && b->byte[i] == 0)){
            if (a->byte[i-offset] > b->byte[i])
	      return 1;
            else
	      return 0;
        }
      }
    }
    return 1;
    } else { 
      int offset = a->size - b->size;
      for (i = 0; i < a->size; i++) {
        if (i < offset) {
          if (!(a->byte[i] == 0))
            return 1;
        } else {
          if (!(a->byte[i] == 0 && b->byte[i-offset] == 0)){
            if (a->byte[i] > b->byte[i-offset])
	      return 1;
            else
	      return 0;
        }
     }
    }
    return 1;
    }
  }
}

/*! Function: lt

 Desc: check if bits type is less than 
       another bits type. can handle bits of any
       size

 Pre: must pass 2 bits type
      
      

 Post: return 0 if a>=b or 1 if a<b
*/
int lt(bits* a, bits* b){
  int i;
  if (eq(a,b))
    return 0;
  if (a->size == b->size) {
    for (i = 0; i < a->size; i++){
      if (!(a->byte[i] == 0 && b->byte[i] == 0)){
        if (a->byte[i] < b->byte[i])
	  return 1;
        else
	  return 0;
      }
    }
    return 1;
  } else { 
    //a < b
    if (a->size < b->size) {
      int offset = b->size - a->size; 
      for (i = 0; i < b->size; i++) {
        if (i < offset) {
          if (!(b->byte[i] == 0))
            return 0;
        } else {
          if (!(a->byte[i-offset] == 0 && b->byte[i] == 0)){
            if (a->byte[i-offset] < b->byte[i])
	      return 1;
            else
	      return 0;
            }
          }
        }
        return 1;
    } else { 
    int offset = a->size - b->size;
      for (i = 0; i < a->size; i++) {
        if (i < offset) {
          if (!(a->byte[i] == 0))
            return 1;
        } else {
          if (!(a->byte[i] == 0 && b->byte[i-offset] == 0)){
            if (a->byte[i] < b->byte[i-offset])
	      return 1;
            else
	      return 0;
        }
        }
        }
        return 1;
    }
  }
}

/*! Function: eq

 Desc: checks if two bits type are equal 
       can handle bits of any size


 Pre: must pass 2 bits type
      
      

 Post: return 0 if a!=b or 1 if a==b
*/
int eq(bits* a, bits* b) {
  int i;
  
  if (a->size == b->size) {
    for (i = 0; i < a->size; i++){
      if (a->byte[i] != b->byte[i])
        return 0;
    }
    return 1;
  
  } else { 
    //a < b
    if (a->size < b->size) { 
      int offset = b->size - a->size;
      for (i = 0; i < b->size; i++) {
        if (i < offset) {
          if ((b->byte[i] != 0x00))
            return 0;
        } else {
          if (a->byte[i-offset] != b->byte[i])
            return 0;
        }
        }
        return 1;
    } else { 
      int offset = (a->size - b->size);
      for (i = 0; i < a->size; i++) {
        if (i < offset) {
          if ((a->byte[i] != 0x00))
            return 0;
        } else {
          if (a->byte[i] != b->byte[i-offset])
            return 0;
        }
        }
        return 1;
        }
    }
  }

// bit ops  

/*! Function: and

 Desc: performs and operation on two bits types
       can handle bits on any size
       

 Pre: must pass 2 bits type
      
      

 Post: return a new bits polynomial
*/
bits* and(bits* a, bits* b) {
  int i;
  if (a->size == b->size) {
    bits* temp = bits_init(a->size);
    for (i = 0; i < a->size; i++) {
      temp->byte[i] = a->byte[i] & b->byte[i];
    }
    return temp;
  } else {    
    if (a->size < b->size) { 
      bits* temp = bits_init(b->size);
      int offset = b->size - a->size;
      for (i = 0; i < b->size; i++) {
        if (i < offset) {
          temp->byte[i] = 0x00;
        } else {
          temp->byte[i] = a->byte[i-offset] & b->byte[i];
        }
      }
      return temp;
    } else { 
      bits* temp = bits_init(a->size);
      int offset = (a->size - b->size);
      for (i = 0; i < a->size; i++) {
        if (i < offset) {
          temp->byte[i] = 0x00;
        } else {
          temp->byte[i] = a->byte[i] & b->byte[i-offset];
        }
      }
      return temp;
    }
  }
}

/*! Function: or

 Desc: performs or operation on two bits types
       can handle bits on any size
       

 Pre: must pass 2 bits type
      
      

 Post: return a new bits polynomial
*/
bits* or(bits* a, bits* b) {
  int i;
  if (a->size == b->size) {
    bits* temp = bits_init(a->size);
    for (i = 0; i < a->size; i++) {
      temp->byte[i] = a->byte[i] | b->byte[i];
    }
    return temp;
  } else {    
    if (a->size < b->size) { 
      bits* temp = bits_init(b->size);
      int offset = b->size - a->size;
      for (i = 0; i < b->size; i++) {
        if (i < offset) {
          temp->byte[i] = b->byte[i];
        } else {
          temp->byte[i] = a->byte[i-offset] | b->byte[i];
        }
      }
      return temp;
    } else { 
      bits* temp = bits_init(a->size);
      int offset = (a->size - b->size);
      for (i = 0; i < a->size; i++) {
        if (i < offset) {
          temp->byte[i] = a->byte[i];
        } else {
          temp->byte[i] = a->byte[i] | b->byte[i-offset];
        }
      }
      return temp;
    }
  }
}

/*! Function: xor

 Desc: performs xor operation on two bits types
       can handle bits on any size
       

 Pre: must pass 2 bits type
      
      

 Post: return a new bits polynomial
*/
bits* xor(bits* a, bits* b) {
  int i;
  int offset;
  

  if (a->size == b->size) {
    bits* temp = bits_init(a->size);
    for (i = 0; i < a->size; i++) {
      temp->byte[i] = a->byte[i] ^ b->byte[i];
    }
    return temp;
  } else {    
    if (a->size < b->size) { 
      bits* temp = bits_initbits(b, b->size);
      offset = b->size - a->size;
      for (i = offset; i < b->size; i++) {
          if (a->byte[i-offset] != 0)
            temp->byte[i] = a->byte[i-offset] ^ b->byte[i];
      }
      return temp;
    } else { 
      bits* temp = bits_initbits(a, a->size);
      offset = a->size - b->size;
      for (i = offset; i < a->size; i++) {
      	  if (b->byte[i-offset] != 0)
            temp->byte[i] = a->byte[i] ^ b->byte[i-offset];
      }
      return temp;
    }
  }
}

/*! Function: shiftl_byte

 Desc: performs byte shift on bits type
       
       

 Pre: must pass bits type and number of byte shifts 
      helper method for shiftl
      

 Post: return pointer to passed in bits polynomial
*/
bits* shiftl_byte(bits* a, int times) {
  int i, j;
  
  if(times == 0) {
    return a;
  }
  memmove((unsigned char*)(&(a->byte[0])),
	   (unsigned char*)(&(a->byte[times])), a->size-times);
  memset((unsigned char*)(&(a->byte[a->size-times])), 0, times);

  return a;
}

/*!  Function: shiftl_bit

 Desc: performs bit shift on bits type
       
       

 Pre: must pass bits type and number of bit shifts 
      helper method for shiftl
      

 Post: return pointer to passed in bits polynomial
*/
bits* shiftl_bits(bits* a, int times) {
  //times will always be < 8
  int j;
  if(times == 0) {
    return a;
  }
  unsigned char firstbit = 0;
  unsigned char byte = 0;
  int i = a->size -1;
  for (; i >= 0; i--) {
    byte = (a->byte[i] << times) | firstbit;
    firstbit = a->byte[i] >> 8-times;
    a->byte[i] = byte; 
  }
  return a;
}

/*!  Function: shiftl

 Desc: performs left shift on bits type
       
       

 Pre: must pass bits type and number of bits to shifts 
      
      

 Post: return a new bits polynomial
*/
bits* shiftl(bits* a, int times) {
  //fast shiftl
  int i;
  
  // finds minimum byte size after shiftl
  int byte_shift = times/8;
  int bit_shift = times%8;
  int high_bit = poly_degree(a); 
  
  int min_bytes = ((high_bit + times)/8)/8*8+8; //bit size will stay multiple of 8
  
  bits* temp = bits_initbits(a, min_bytes);
  temp = shiftl_bits(temp, bit_shift);
  temp = shiftl_byte(temp, byte_shift);
  
  return temp;
}

/*!  Function: shiftr

 Desc: performs right shift on bits type
       
       

 Pre: must pass bits type and number of bits to shifts 
      
      

 Post: return a new bits polynomial
*/
bits* shiftr(bits* a, int times) {
  int j, i;
  bits* temp = bits_initbits(a, a->size);
  for (j = 0; j < times; j++) {
    unsigned char lastbit = 0;
    unsigned char byte = 0;
    for (i = 0; i < temp->size; i++) {
      /* Starting from left shift bit right and
	 OR with bitmask last bit of byte of 
	 previous byte-> First bit in first byte is 
	 always 1'b0
       */
      byte = (temp->byte[i] >> 1) | lastbit;
      lastbit = (temp->byte[i]) << 7;
      temp->byte[i] = byte;
     }
  }
  return temp;
}

/*!  Function: bit_array_print_report

 Desc: runs tests on bit_array functions
       and prints pass or fail
       leaks memory 

 Pre: none 
      
      

 Post: none
*/

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
    
    
    //lte - Less than equal to
    printf("lte: ");
    
    1==lte(bits_initlong(555436),bits_initlong(555436))&&1==lte(bits_initlong(555436),bits_initlong(555436))&&0==lte(bits_initlong(8),bits_init(16))&&1==lte(bits_init(8),bits_initlong(16))&&0==lte(bits_initlong(100056658),bits_initlong(1000))?printf("ok\n"):printf("failed\n");

    //gte - Greater than equal to
    printf("gte: ");
    1==gte(bits_initlong(555436),bits_initlong(555436))&&0==gte(bits_initlong(8),bits_initlong(9))&&1==gte(bits_initlong(8),bits_init(16))&&0==gte(bits_init(8),bits_initlong(16))&&1==gte(bits_initlong(100056658),bits_initlong(1000))?printf("ok\n"):printf("failed\n");

    //gt - Greater than
    printf("gt: ");
    0==gt(bits_initlong(555436),bits_initlong(555436))&&0==gt(bits_initlong(8),bits_initlong(9))&&1==gt(bits_initlong(8),bits_init(16))&&0==gt(bits_init(8),bits_initlong(16))&&1==gt(bits_initlong(100056658),bits_initlong(1000))?printf("ok\n"):printf("failed\n");

    //lt - Less than
    printf("lt: ");
    0==lt(bits_initlong(555436),bits_initlong(555436))&&1==lt(bits_initlong(8),bits_initlong(9))&&0==lt(bits_initlong(8),bits_init(16))&&1==lt(bits_init(8),bits_initlong(16))&&0==lt(bits_initlong(100056658),bits_initlong(1000))?printf("ok\n"):printf("failed\n");

    //eq - Equal to
    printf("eq: ");
    (1==eq(bits_initlong(555436),bits_initlong(0x879AC))&&0==eq(bits_initlong(555436),bits_init(16))&&1==eq(bits_init(8),bits_init(16))&&1==eq(bits_init(16),bits_init(8))&&0==eq(bits_initlong(100056798),bits_initlong(0x5F7DE))?printf("ok\n"):printf("failed\n"));

    //----------
    //ops
    //----------


    //and
    printf("and: ");
    eq(bits_initlong(0x000b030a28242002),and(bits_initlong(4701532727728676918),bits_initlong(453609631608285195)))&&eq(bits_initlong(0x00),and(bits_initlong(4701532727728676918),bits_init(8)))&&!eq(bits_initlong(0x000b030a28242002),and(bits_initlong(4701532727728676918),bits_initlong(453609631608222222)))?printf("ok\n"):printf("failed\n");
    

    //or
    printf("or: ");
    eq(bits_initlong(0x477fbb9f7eb5283f),or(bits_initlong(4701532727728676918),bits_initlong(453609631608285195)))&&eq(bits_initlong(4701532727728676918),or(bits_initlong(4701532727728676918),bits_init(8)))&&!eq(bits_initlong(0x477fbb9f7eb5283f),or(bits_initlong(4701532727728676918),bits_initlong(453609631608285206)))?printf("ok\n"):printf("failed\n");

    //xor
    printf("xor: ");
    eq(bits_initlong(0x4774b8955691083d),xor(bits_initlong(4701532727728676918),bits_initlong(453609631608285195)))&&
    eq(bits_initlong(4701532727728676918),xor(bits_initlong(4701532727728676918),bits_init(16)))&&!eq(bits_initlong(0x477fbb9f7eb5283f),xor(bits_initlong(4701532727728676918),bits_initlong(453609631608285206)))?printf("ok\n"):printf("failed\n");

    //shiftl - shift left
    printf("shiftl: ");
    eq(bits_initlong(0x00110011000000000),shiftl(bits_initlong(0x110011),36))&&eq(bits_initlong(0x8000000000000000),shiftl(bits_initlong(0x1),63))&&!eq(bits_initlong(0x477fbb9f7eb5283f),shiftl(bits_initlong(4701532727728676918),4))?printf("ok\n"):printf("failed\n");

    //shiftr - shift right
    printf("shiftr: ");
    eq(bits_initlong(0x413f331b2824203),shiftr(bits_initlong(4701532727728676918),4))&&!eq(bits_initlong(0x477fbb9f7eb5283f),shiftr(bits_initlong(4701532727728676918),4))?printf("ok\n"):printf("failed\n");
												  
}
