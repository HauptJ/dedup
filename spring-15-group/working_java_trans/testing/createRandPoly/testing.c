//test cases
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "testing.h"


int main() {
  //testing 
  printf("createRandomLongPoly() Output\n");
  unsigned long polyLong = createRandomLongPoly(); //createRandomLongPoly() works :D
  printf("Output from createRandomLongPoly() = ");
  printf("\n%lu\n", polyLong); //output polyLong
  
  printf("toStringPoly() Output\n");
  char* polyString;
  polyString = toStringPoly(polyLong); //now testing toStringPoly
  printf("Output from toStringPoly() = ");
  printf("\n%lu\n", polyString);
  
  printf("toStringHex() output\n");
  unsigned long *polyLongHex;
  polyLongHex = toStringHex(polyLong);
  printf("Output from toStringHex() = "); 
  printf("\n%lu\n", polyLongHex); //output hex - need fix?
  
  return 0;
}

//createRandomLongPoly()
unsigned long createRandomLongPoly() { 

	srand(time(NULL)); // seed for random number generator

    const int arraySize=8;
    unsigned char randomNum;
    unsigned char polyTempArr[arraySize];

    // populate array
	int i = 0;
    for (; i < arraySize; i++)
    {
        randomNum= rand()%256;
        polyTempArr[i]= randomNum;
    }
     //convert the array to a long
    unsigned long polyLong;
    memcpy (&polyLong, polyTempArr, sizeof (long)); // nice approach :)
	printf("Output generated in createRandomLongPoly()");
    printf("\n%lu\n", polyLong );

    return polyLong;
}


char *toStringPoly(unsigned long l){
    //x^n+x^(n-1)...+X+C
    char* tempBinString;
    tempBinString=(char*)malloc((sizeof(l)*2+1)*4);
    tempBinString=toStringBin(l);  //call to toStringBin()

     char* polyString;
    polyString=(char*)malloc((sizeof(l)*2+1)*40); // it takes 4 bits to represent one hex so the sting needs be 4 times as long
    int i = 0;
    for (; i < strlen(tempBinString); i++){ // added semicolon to front of for loop
        if (tempBinString[i]=='1')
        {
            if (strlen(polyString)!=0) // append plus every time except for greatest degree x
            {
                strcat(polyString,"+");
            }
            
            if (i==strlen(tempBinString)-2)// regular x
            {
                strcat(polyString,"x");
            }

            else if (i==strlen(tempBinString)-1)// rightmost char in string, this is our C term
            {
                strcat(polyString,"1");
            }
            else
            {
                char *strToAppend;
				// look over possibly use strcat
                strToAppend=(char*)malloc(2+numDigits(i%strlen(tempBinString)+1));// 2+ for "x^" +1 for term char //call to numDigits()
				//malloc() http://www.cplusplus.com/reference/cstdlib/malloc/
                // then figure out how many digits to store
                snprintf (strToAppend,sizeof(strToAppend),"x^%lu",(unsigned long)strlen(tempBinString)-i%strlen(tempBinString));
                strcat(polyString,strToAppend);
                free(strToAppend);
            }
        }
    }
    free(tempBinString);
	printf("Output generated in toStringPoly() ");
	printf("\n%lu\n", polyString);
    return polyString;
}

char *toStringBin(unsigned long l){
    //hex is a nice starting point for binary
    char* tempHexString;
    tempHexString=(char*)malloc(sizeof(l)*2+1); // two hex chars make up 1 byte, add 1 for term char)
    tempHexString=toStringHex(l);

    //our main string
    char* binString;
    binString=(char*)malloc((sizeof(l)*2+1)*4); // it takes 4 bits to represent one hex so the sting needs be 4 times as long
    int i = 0;
    for (; i < strlen(tempHexString);i++)// forgot ; at beginning of for loop
    {
        switch(tempHexString[i])
        {
            case '0':strcat(binString,"0000");break;
            case '1':strcat(binString,"0001");break;
            case '2':strcat(binString,"0010");break;
            case '3':strcat(binString,"0011");break;
            case '4':strcat(binString,"0100");break;
            case '5':strcat(binString,"0101");break;
            case '6':strcat(binString,"0110");break;
            case '7':strcat(binString,"0111");break;
            case '8':strcat(binString,"1000");break;
            case '9':strcat(binString,"1001");break;
            case 'A':strcat(binString,"1010");break;
            case 'B':strcat(binString,"1011");break;
            case 'C':strcat(binString,"1100");break;
            case 'D':strcat(binString,"1101");break;
            case 'E':strcat(binString,"1110");break;
            case 'F':strcat(binString,"1111");break;
        }
    }
    free(tempHexString);
    return binString;

}

unsigned numDigits(const unsigned n) { // NOT MINE http://stackoverflow.com/questions/3068397/finding-the-length-of-an-integer-in-c 
    if (n < 10) return 1;
    return 1 + numDigits(n / 10);
}

//toStrings
char *toStringHex(unsigned long l){
    char *hexString;
    hexString=(char*)malloc(sizeof(l)*2+1); // two hex chars make up 1 byte, add 1 for term char)
    snprintf (hexString,sizeof(l)*2+1,"%lX",l);
    return hexString;
}

