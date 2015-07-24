#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "biginteger.h"

integer integer_init(){
  integer ret;
  bzero((unsigned char*)ret.byte, BYTES);
  return ret;
}



integer integer_initint(integer in){
  integer ret = integer_init();
  memcpy(ret.byte, in.byte, BYTES);
  return ret;
}

integer integer_initones(){
  integer ret = integer_init();
  memset(ret.byte, 255, BYTES);
  return ret;
}


integer integer_initval(unsigned long int val){
  //set integer to val, strip off leading bits if 
  //doesn't fit
  integer ret = integer_init();

  unsigned int intoffset = sizeof(unsigned long int);
  unsigned int temp = 0;

  //swap byte order to make copying easier
  if (endianness() == LITTLE){
    temp |= swapbytes((unsigned int)val);
    val = swapbytes((unsigned int)(val >> 32)) |  (((unsigned long int)temp) << 32);
  }

  //if bytes are fewer than input...
  if (BYTES < sizeof(unsigned long int)){
    intoffset = sizeof(unsigned long int)-BYTES;
    val = val << (intoffset*8);   
    memcpy((unsigned char*)(&(ret.byte[0])), 
	   (unsigned char*)(&val), BYTES);
  }
  else{
    memcpy((unsigned char*)(&(ret.byte[BYTES-intoffset])), 
	   (unsigned char*)(&val), intoffset);
  }
  return ret;
}

unsigned int swapbytes(unsigned int val){
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
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


//return a << 1
integer integer_shiftl(integer a){
  int i = BYTES-1;
  integer ret = integer_initint(a);
  unsigned char highbit = 0;
  unsigned char curbyte = 0;
  for (; i >= 0; i--){
    curbyte = (ret.byte[i] << 1) | highbit;    
    highbit = ret.byte[i] >> 7;
    ret.byte[i] = curbyte;
  }
  return ret;
}

integer integer_shiftl_times(integer a, int times){
  int i;
  integer ret = integer_initint(a);
  for (i = 0; i < times; i++){
    ret = integer_shiftl(ret);
  }
  return ret;
}


//return a >> 1
integer integer_shiftr(integer a){
  int i = 0;
  unsigned char highbit = 0;
  unsigned char curbyte = 0;
  integer ret = integer_initint(a);
  for (; i < BYTES; i++){
    curbyte = (ret.byte[i] >> 1) | highbit;
    highbit = (ret.byte[i] & 0x01) << 7;
    ret.byte[i] = curbyte;
  }
  return ret;
}


//return 1 if a >= b
int integer_gte(integer a, integer b){
  int i;
  if (integer_eq(a,b))
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


//return 1 if a > b
int integer_gt(integer a, integer b){
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

//return 1 if a < b
int integer_lt(integer a, integer b){
  int i;
  for (i = 0; i < BYTES; i++){
    if (!(a.byte[i] == 0 && b.byte[i] == 0)){// && i != BYTES-1){
      if (a.byte[i] < b.byte[i])
	return 1;
      else
	return 0;
    }
    /*    else if (i == BYTES-1) 
      if (a.byte[i] < b.byte[i])
	return 1;
    */
  }
  return 0;
}


//return 1 if a <= b
int integer_lte(integer a, integer b){
  int i;
  if (integer_eq(a,b))
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


//return a & b
integer integer_and(integer a, integer b){
  integer ret = integer_init();
  int i = 0;
  for (; i < BYTES; i++){
    ret.byte[i] = a.byte[i] & b.byte[i];
  }
  return ret;
}


//return a | b
integer integer_or(integer a, integer b){
  integer ret = integer_init();
  int i = 0;
  for (; i < BYTES; i++){
    ret.byte[i] = a.byte[i] | b.byte[i];
  }
  return ret;
}


//return a ^ b
integer integer_xor(integer a, integer b){
  integer ret = integer_init();
  int i = 0;
  for (; i < BYTES; i++){
    ret.byte[i] = a.byte[i] ^ b.byte[i];
  }
  return ret;
}



//return a*b
integer integer_mult(integer a, integer b){
  integer ret = integer_init();
  integer zero = integer_init();
  integer temp_a = integer_initint(a);
  integer temp_b = integer_initint(b);
  int i,j;
  for (i = 0; i < (BYTES-1)*8; i++){
    if ((temp_b.byte[BYTES-1] & 0x01) == 0x01)
      ret = integer_add(ret, temp_a, NULL);
    /*
    integer_printhex(temp_a);
    integer_printhex(temp_b);
    integer_printhex(ret);
    printf("\n");*/
    if (integer_eq(b, zero))
      return ret;

    temp_a = integer_shiftl(temp_a);
    temp_b = integer_shiftr(temp_b);
  }
  return ret;
}

//returns N%D
integer integer_mod(integer dividend, integer divisor){
  int num_bits = 0;
  unsigned char q = 0;
  unsigned char bit = 0;
  int i = 0;

  integer remainder = integer_init();
  integer tempdividend = integer_initint(dividend);
  integer d = integer_init();
  integer t = integer_init();

  if (integer_eq(divisor,remainder)) // check whether divisor is 0
    return remainder;

  if (integer_gt(divisor, dividend)){
    return dividend;
  }
  
  if (integer_eq(divisor, dividend)){
    return integer_initval(1);
  }

  num_bits = BYTES*8;

  while (integer_lt(remainder, divisor)){
    bit = (dividend.byte[0] & 0x80) >> 7;
    remainder = integer_shiftl(remainder);
    remainder.byte[BYTES-1] = remainder.byte[BYTES-1] | bit;
    d = integer_initint(dividend);
    dividend = integer_shiftl(dividend);
    num_bits--;
    if (num_bits < 0)
      fprintf(stderr, "uh oh\n");
  }


  /* The loop, above, always goes one iteration too far.
     To avoid inserting an "if" statement inside the loop
     the last iteration is simply reversed. */

  dividend = integer_initint(d);
  remainder = integer_shiftr(remainder);
  num_bits++;

  for (i = 0; i < num_bits; i++) {
    bit = (dividend.byte[0] & 0x80) >> 7;
    remainder = integer_shiftl(remainder);
    remainder.byte[BYTES-1] = remainder.byte[BYTES-1] | bit;
    
    t = integer_sub(remainder, divisor);
    q = !((t.byte[0] & 0x80) >> 7);
    dividend = integer_shiftl(dividend);
    //    *quotient = (*quotient << 1) | q;
    if (q) {
       remainder = t;
     }
  }


  return remainder;
}


//returns N%D... slowly
integer integer_slowmod(integer N, integer D){
  integer zero = integer_init();
  integer rem = integer_initint(N);

  if (integer_lt(N,D))
    return rem;
  if (integer_eq(N,D))
    return zero;

  int it = 1;
  integer temp = integer_initint(D);

  while (integer_gte(rem, D)){
    rem = integer_sub(rem, D);
  }
  return rem;
}



//returns 2s complement of a
integer integer_twoscomp(integer a){
  integer ret = integer_init();
  integer one = integer_initval(1);

  int i,j;
  for (i = 0; i < BYTES; i++){
    ret.byte[i] = ~a.byte[i];
  }

  ret = integer_add(ret, one, NULL);
  return ret;
}


//returns a - b
integer integer_sub(integer a, integer b){
  integer twoscompb = integer_twoscomp(b);
  return integer_add(a, twoscompb, NULL);

}


//return a + b
integer integer_add(integer a, integer b, int* overflow){
  integer ret = integer_init();

  unsigned long int ia, ib, temp, carry = 0;
  unsigned char sum_byte;
  
  int i;
  for (i = (BYTES-1); i >= 0; i--){
    ia = a.byte[i];
    ib = b.byte[i];
    temp = ia + ib;
    //printf("%u: adding %u %u = %u\n", i, ia, ib, temp); 
    sum_byte = (unsigned char)(temp+carry);
    //printf("%u: truncated sum %u\n", i, sum_byte);
    ret.byte[i] = sum_byte;
    carry = (unsigned char)((temp + carry) >> 8);
    //printf("%u: carry %u\n", i, carry);
  }
  if (carry > 0 && overflow != NULL)
    *overflow = 1;
  return ret;
}


//return 1 if a == b
int integer_eq(integer a, integer b){
  int i;
  for (i = 0; i < BYTES; i++){
    if (a.byte[i] != b.byte[i])
      return 0;
  }
  return 1;
}


integer integer_genrandprime(){
  integer temp = integer_init();
  int cur; unsigned char rand_byte = 0; 
  srand(17); //change (TODO), below tested ok

  do{
    for (cur = 0; cur < BYTES; cur++){
      rand_byte = rand() % 256;
      if (cur == 0)
	rand_byte |= (1 << 7);
      if (cur == BYTES-1)
	rand_byte |= 1;
      temp.byte[cur] = rand_byte;
    } 
  } while (integer_isprime(temp) != 1);

  printf("found prime!\n");
  integer_printhex(temp);

  return temp;
}

int integer_isprime(integer n){
  integer val_0 = integer_initval(0);
  integer val_1 = integer_initval(1);
  integer val_2 = integer_initval(2);
  integer val_3 = integer_initval(3);
  integer val_5 = integer_initval(5);
  integer val_6 = integer_initval(6);
  integer cur = val_5;

  //temporarily here
  if (integer_lte(n, val_3)) {  //n <= 3
    return integer_gte(n, val_2);
  }
  printf("check divisible by 2 or 3, input: ");
  integer_printhex(n);
  if (integer_eq(integer_mod(n, val_2), val_0) || integer_eq(integer_mod(n, val_3), val_0)){
    printf("is this divisible by 2 or 3? ");
    integer_printhex(n);
    return 0;
  }
  printf("keep checking\n");

  for (; integer_lte(integer_mult(cur, cur), n) ; cur = integer_add(cur, val_6, NULL)){
    if (integer_eq(integer_mod(n, cur), val_0) ||
	integer_eq(integer_mod(n, integer_add(cur,val_2,NULL)), val_0)){
      return 0;
    }
  }
  /*slow
  for (; integer_lte(integer_mult(cur, cur), n) ; cur = integer_add(cur, val_2, NULL)){
    if (integer_eq(integer_mod(n, cur), val_0)){
      return 0;
    }
  }
  */
  return 1;
}


/* int 
gcd ( int a, int b )
{
  int c;
  while ( a != 0 ) {
     c = a; a = b%a;  b = c;
  }
  return b;
}
*/

integer integer_gcd(integer a, integer b){
  integer zero = integer_init();
  while(!integer_eq(a, zero)){
    integer c = integer_initint(a);
    a = integer_mod(b, a);
    b = integer_initint(c);
  }
  return b;
}

void integer_printhex(integer in){
  int cur;
  printf("0x");
  for (cur = 0; cur < BYTES; cur++){
    printf("%x", in.byte[cur]);
  }
  printf("\n");
}

void integer_gen_test_print(){
  int i = 1;
  int j = 0;
  printf("endianness: %s\n", (endianness()==BIG)?"big":"little"); //little on mac :(

  printf("gcd: ");
  integer_eq(integer_gcd(integer_initval(214748364600), integer_initval(1293120)), integer_initval(360))?printf("ok\n"):printf("failed\n");

  printf("random integer\n");
  printf("mod: ");
  (integer_eq(integer_mod(integer_initval(0xfe2c1ead5a2e3c1), integer_initval(0xff)), integer_initval(0xbc)))?printf("ok\n"):printf("failed\n");
  //4294967291
  printf("isprime: %s", (integer_isprime(integer_initval(3915039707)))?"ok\n":"failed\n");

  printf("generating\n");
  //integer test = integer_genrandprime();
  printf("\n");

  integer test2 = integer_init();
  printf("zeroed integer ");
  for (i = 0; i < BYTES; i++)
    printf("%u ", test2.byte[i]);
  printf("\n");
  
  integer test3 = integer_initval(0xdeadbeefdeadbeef);
  printf("value initialized integer 0xdeadbeefdeadbeef:\n");
  for (i = 0; i < BYTES; i++)
    printf("%x ", test3.byte[i]);
  printf("\n");

  integer test4 = integer_initval(3219124214);
  integer test508 = integer_initval(508);
  integer test255 = integer_initval(255);
  printf("lte %s\n", (integer_lte(test4,test3))?"ok":"failed");
  printf("lte %s\n", (integer_lte(test3,test3))?"ok":"failed");
  printf("gte %s\n", (integer_gte(test3,test4))?"ok":"failed");
  printf("gte %s\n", (integer_gte(test3,test3))?"ok":"failed");
  printf("eq  %s\n", (integer_eq(test3,test3))?"ok":"failed");
  printf("lt  %s\n", (integer_lt(test255,test508))?"ok":"failed");

  integer test5 = integer_initval(4294967295);
  integer test7 = integer_add(test5, test5, NULL);
  if (BYTES < 5)
    printf("skipping addition, not enough bytes\n");
  else
    (!integer_eq(test7, integer_initval(8589934590)))?
      printf("add failed\n"):printf("add ok\n");

  printf("shiftl %s\n",
	 (integer_eq(integer_shiftl(integer_initval(128)), integer_initval(256)))?
	 "ok":"failed");
    
  printf("shiftr %s\n",
	 (integer_eq(integer_shiftr(integer_initval(256)), integer_initval(128)))?
	 "ok":"failed");

  integer test10 = integer_initint(test5);
  printf("initint %s\n", (integer_eq(test5, test10))?"ok":"failed");


  printf("or %s\n",
	 (integer_eq(integer_or(integer_initval(349525),
			   integer_initval(699050)),
		  integer_initval(1048575))?
	  "ok":"failed"));

  printf("and %s\n",
	 (integer_eq(integer_and(integer_initval(349525),
			   integer_initval(699050)),
		  integer_initval(0))?
	  "ok":"failed"));


  printf("mult %s\n",
	 (integer_eq(integer_mult(integer_initval(5555555),
			    integer_initval(555)),
		  integer_initval(3083333025)))?
	 "ok":"failed");
  //integer_printhex(integer_mult(integer_initval(5555555),
  //			  integer_initval(555)));
  
  printf("sub %s\n",
	 (integer_eq(integer_sub(integer_initval(5555555),
			   integer_initval(4444444)),
		  integer_initval(1111111)))?
	 "ok":"failed");

  printf("mod %s\n",
	 (integer_eq(integer_mod(integer_initval(15),
			   integer_initval(5)),
		  integer_initval(0)))?
	  "ok":"failed");
}
