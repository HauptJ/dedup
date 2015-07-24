//   File: main.c
//   Name: Joshua Haupt, Nick Kirkpatrick, Spencer ???
//   Date: Spring 15
// Course: CS 495 - Crk Group
//   Desc: This file includes the main / driver function.
//         
//         
//         
//
//         Other files required: main.c, polynomials.c, polynomials.h, bit_array.h, toStringFctns.h,
//                               toStringFctns.c


#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h> // fileCheck
//#include <string.h>
#include "polynomials.h"
#include "bit_array.h"
#include "toStringFctns.h"
#include "fingerprint.h"

#define MAX_FILES 1000
/*!
 * Desc:
 * simply prints some help function about how to use the driver
 * 
 * called when no args are given
 *
 * Pre: none
 * Post: prints usage info as seen in terminal
 */
void print_usage() {
    printf(" \n\n ");
    printf("fp - Rabin Fingerprinting Driver \n");
    printf("Usage: \n");
    printf("----------\n");
    printf("-g <length in bytes> <== generates an irreducible polynomial with stated length\n\n");
    printf("-i <polynomial as hex string> -f <some files> \n [-b <some number of bits>] [-w <size of window>] \n\n");
    printf("-i <polynomial as hex string> -s <some bytes in stdin> \n [-b <some number of bits>] [-w <size of window>] \n\n");
    printf("[b is optional bitmask, default is 13]\n");
    printf("[w is optional window size, default is poly byte size + 4 bytes]\n");
    printf("[w must be > poly size and b must be < 64]\n");
    printf(" \n\n ");
    printf(" generate example \n");
    printf(" ./fp -g 13 ");
    printf(" stdin example: \n");
    printf(" ./fp -i \"ABB2CDC69BB454110E827441213DDC87\" -s \"aahfhafafa\" -b 13 -w 18 \n ");
    printf(" file example: \n");
    printf(" ./fp -i \"ABB2CDC69BB454110E827441213DDC87\" -f test.txt -b 13 -w 18 \n ");
    printf(" \n\n ");
   
    printf("----------\n");
    printf("OUTPUT FORMAT \n");
    printf("<input>: <chunk start offset> <chunk end offset> <chunk size, basically end-start> <SHA-1 of chunk>\n");
    printf("so, for example:\n\n ");
    printf("testfile: 0 123 123 8ea68a9605c933e1579d5737f31514db429c660f \n ");
    printf("testfile: 123 567 444 44e865f163593af14247bf63b8c40c6d01b16ad8 \n ");
    printf("testfile: 567 1000 433 4a0f892bad8e9695a86acc25483aed569ddfdf5d \n ");
    printf("or\n ");
    printf("stdin: 0 555 555 8ea68a9605c933e1579d5737f31514db429c660f \n ");
    printf("etc...\n");
    printf("----------\n");
    printf("SAMPLE POLYNOMIALS \n");
    printf("1 byte: CB \n");
    printf("8 bytes: EF5ADD51E5ED1515 \n");
    printf("12 bytes: 9F231EA81C7B64C514735AC5 \n");
    printf("13 bytes: E56A85CD8D9B3A9E2C7EDB99C1 \n");
    printf("16 bytes: E61280256DDD9A9F680A692E3EBBDE37 \n");
    printf("24 bytes: F9881AAD6BAB6773D510C60E20E19D8EB3C09A18FBA78987 \n");
    printf("32 bytes: 88D69FC00F636FDA9F7D9B456616349F26420FB6266E272508295B6F24C62C2D \n");
    printf("40 bytes: E5D19362371B51B03FEDB0544CCFE6F1F1E009788E08DA9C9505803F4004A625D53A880D55D9BD95 \n");
    printf("48 bytes: A5BFC5E7DFFC28123195353C479DB58ADF4AA58B50FDA68B4C0F0106C3642C6824F14F03EE78151F0D4A5B55E811DFC7 \n");
    printf("56 bytes: D1E4EFF97D4396D3C2411811820BFC447BF82565DD75E67DC8EE8B32F4633AC54729BFC46D55972F96AF4018BA3C5C3635819B12F6818FBF \n");
    printf("64 bytes: B31CFA58B876864F8B69CE507F3F802D083FA84AA8D6D7EB5A33CC11B31F55673B4FBFF3C6454251AE10A12D4F215A576103A209D979F433ACC04560E09AC71B \n");
    
    
}

//file check
//fileCheck in fingerprint.c
/*!
 * Desc:
 * checks to make sure a file is both openable AND readable. Takes a string as
 * a the file name to check
 
 * Pre: pointer to file *fname
 * Post: returns 1 if file exists and is readable, 0 if file either does not exist or is not readable
 */
int fileCheckM(char *fname) {
  FILE *file;
    if (file = fopen(fname, "r")) {
      if(access(fname, F_OK) !=-1) {
        //printf("File is openable, and readable\n");
          return 1;
        } else {
          printf("File is openable but something is not right\n");
          return 0;
        }
          fclose(file);
    }
    printf("File is not openable / found\n");
    return 0;
}

/*!
 * Desc:
 * main / driver
 *
 * Pre: arguments from the command line 
 * refer to print_usage()
 * Post: calls necessary functions based off command line arguments
 */

int main (int argc, char*argv[]){ // will take a list of files to be fingerprinted

//bit_array_print_report();

  if(argc == 1) {print_usage();} // nothing specied

bits *bitmask, *poly;
char *files[1000];
int file_count = 0;
char* stdin_data;
int option;
int generate = -1, irreducible = -1, file = -1, standardin = -1, mask = -1, window = -1; 

  while ((option = getopt(argc, argv,"g:i:f:s:b:w:")) != -1) { // looping through specified opts 
    switch (option) {
      case 'g' : generate = atoi(optarg);
      break;
      case 'i' : irreducible = 0;
                 poly = bits_initstring(optarg);
      break;
      case 'f' : file = 0; 
      //get all files
     
      int index = optind-1;
      while(index < argc) {
        
        if(argv[index][0] != '-')
          files[file_count] = argv[index];
        else 
          break;
       
        file_count++;
        index++;
        if (file_count == MAX_FILES) 
          break;
      }
      break;	 
      case 's' : standardin = 0;
        stdin_data = optarg;
      break;
      case 'b' : mask = atoi(optarg);
      break;
      case 'w' : window = atoi(optarg);
      break;
      default: print_usage(); 
        exit(EXIT_FAILURE);
    }      
  }  

  if(generate > 0) {
    srand(time(0));
    if (generate > 32)
      printf("This may take awhile...\n");
    printf("%s\n",toStringHex(*createIrreducible(generate)));
  }
  
  if (irreducible == 0) {
    if (standardin < 0 && file < 0) {print_usage();}
  
    //set window
    if (window > 0) {
       if (window <= poly->size) {
       	 print_usage();
       	 exit(EXIT_FAILURE);
       }
    } else {
      //default poly->size+4
      window = poly->size+4;
    }
    
    //set bitmask
    if (mask > 0) {
      long max = ~0;
      long bits = ~(max << mask);
      bitmask = bits_initlong(bits);
    } else {
      //defualt 13
      bitmask = bits_initlong(0x1FFF);
    }
    if (file == 0) {
      //fingerprint i
      int i;
      for (i=0;i<file_count;i++) {
        //check file exist
        if (fileCheckM(files[i])) {
          chunks* hashes = find_fingerprints_file(files[i], window, poly, bitmask, 0, 0);
          print_chunks(hashes);

          //free data
          free_chunks(hashes);
        }
      }
    }
    
    //fingerprint std
    if (standardin == 0) {
      chunks* hashes = find_fingerprints_bytes(stdin_data, strlen(stdin_data), window, poly, bitmask, 0, 0);
      print_chunks(hashes);

      //free data
      free_chunks(hashes);
    }
    //free data
    free(poly->byte);
    free(poly);
    free(bitmask->byte);
    free(bitmask);
  }
  
  
  return 0;
}


