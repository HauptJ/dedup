//   File: toStringFctns.c
//   Name: Joshua Haupt, Nick Kirkpatrick, Spencer ???
//   Date: Spring 15
// Course: CS 495 - Crk Group
//   Desc: This file includes the polynomial toString Functions. 
//         
//         
//         
//
//         Other files required: main.c, polynomials.h, bit_array.h, toStringFctns.h

//#include "bit_array.h" //there just in case
#include "toStringFctns.h"

/*---toStrings---*/

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

char *toStringHex(unsigned long l){
    char *hexString;
    hexString=(char*)malloc(sizeof(l)*2+1); // two hex chars make up 1 byte, add 1 for term char)
    snprintf (hexString,sizeof(l)*2+1,"%lX",l);
    return hexString;
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

char *toStringBin(unsigned long l){
    //hex is a nice starting point for binary
    char* tempHexString;
    tempHexString=(char*)malloc(sizeof(l)*2+1); // two hex chars make up 1 byte, add 1 for term char)
    tempHexString=toStringHex(l);

    //our main string
    char* binString;
    binString=(char*)malloc((sizeof(l)*2+1)*4); // it takes 4 bits to represent one hex so the sting needs be 4 times as long
    int i = 0;
    for (;i < strlen(tempHexString);i++)
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

unsigned numDigits(const unsigned n) { // NOT MINE http://stackoverflow.com/questions/3068397/finding-the-length-of-an-integer-in-c
    if (n < 10) return 1;
    return 1 + numDigits(n / 10);
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

char*toStringPoly(unsigned long l){// backwards //space between char* toStringPoly maybe?
    //x^n+x^(n-1)...+X+C
    char* tempBinString;
    tempBinString=(char*)malloc((sizeof(l)*2+1)*4);
    tempBinString=toStringBin(l);

     char* polyString;
    polyString=(char*)malloc((sizeof(l)*2+1)*40); // it takes 4 bits to represent one hex so the sting needs be 4 times as long
    int i = 0;
    for (;i < strlen(tempBinString);i++){
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
                strToAppend=(char*)malloc(2+numDigits(i%strlen(tempBinString)+1));// 2+ for "x^" +1 for term char
                // then figure out how many digits to store
                snprintf (strToAppend,sizeof(strToAppend),"x^%lu",(unsigned long)strlen(tempBinString)-i%strlen(tempBinString));
                strcat(polyString,strToAppend);
                free(strToAppend);
            }
        }
    }
    free(tempBinString);
    return polyString;
}
