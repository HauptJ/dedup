
//   File: fingerprint.c
//   Name: Joshua Haupt, Nick Kirkpatrick, Spencer Smith
//   Date: Spring 15
// Course: CS 495 - Crk Group
//   Desc: This file includes functions and data structures for fingerprinting.
//

#include "polynomials.h"
#include "bit_array.h"
#include "fingerprint.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <openssl/sha.h>



/*! file check
fileCheckM - (used in driver) - in main.c

desc: checks if file exists and is readable

pre (input): pointer to file

post (return): 1 - true if file is readable, 0 - false if file is not readable
*/
int fileCheck(char *fname) {
    FILE *file;
    if (file = fopen(fname, "r")) {
        if(access(fname, F_OK) !=-1) {
            printf("File is openable, and readable\n");
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


//chunks

/*! Function: init_chunks

 Desc: creates chunks type to store chunk
       information. chunks is a linked list
       that stores chunk types

 Pre: none



 Post: return a new chunks type
*/
chunks* init_chunks() {
  chunks* temp = malloc(sizeof(chunks));
  temp->size = 0;
  temp->fingers = 0;

  temp->head = init_node(NULL, NULL, NULL);
  temp->tail = init_node(temp->head, NULL, NULL);
  temp->head->next = temp->tail;

  return temp;
}

/*! Function: init_node

 Desc: creates node type and adds it to the
       chunks linked list


 Pre: must pass in previous node pointer,
      next node pointer and chunk data pointer


 Post: returns a new node type
*/
node* init_node(node* previous, chunk* data, node* next) {
  node* temp = malloc(sizeof(node));
  temp->previous = previous;
  temp->data = data;
  temp->next = next;

  return temp;
}

/*! Function: add_chunk

 Desc: add chunk to chunks data structure



 Pre: must pass in pointer to chunks type,
      start offset and end offset


 Post: return a pointer to passed in chunks type
*/
chunks* add_chunk(chunks* data, int start, int end) {
  chunk* temp_chunk = malloc(sizeof(chunk));
  temp_chunk->chunk_start = start;
  temp_chunk->chunk_end = end;
  temp_chunk->size = end - start + 1; // 0 based

  // create node
  node* temp = init_node(data->tail->previous, temp_chunk, data->tail);
  data->tail->previous->next = temp;
  data->tail->previous = temp;
  data->size += (end-start);
  data->fingers++;
  return data;
}

//fingerprint

/*! Function: find_fingerprints_file

 Desc: fingerprints file with a specific window size,
       bitmask, polynomial
       min and max not yet implemented

 Pre: must pass in file name, window_size, poly, bitmask



 Post: return a new chunks type
*/
chunks* find_fingerprints_file(char* file, int window_size, bits* poly, bits* bitmask,int min, int max) {
  chunks* temp = init_chunks();
  temp->filename = file;
  FILE *fp;
  fp = fopen(file, "r");
  long chunk_start = 1;
  long chunk_end = 0;

  //find file size
  long pos = ftell(fp);
  fseek(fp, 0, SEEK_END);
  long length = ftell(fp);
  fseek(fp, pos, SEEK_SET);
  length -= pos;

  bits* window = bits_init(window_size);
  bits* finger;

  int i = 0;
  char c = fgetc(fp);

  while(i < length) {
    window = push_byte_window(c, window);
    finger = poly_mod(window, poly);

    /*if (eq(and(window, bitmask), bits_initlong(0l)) &&
    	chunk_end - chunk_start > min || chunk_end - chunk_start > max) {*/
      bits* and_temp = and(finger, bitmask);
      chunk_end++;

      if (eq(and_temp, bitmask)) {


      temp = add_chunk(temp, chunk_start, chunk_end);
      // debugging

      // reset finger
      //free(finger->byte);
      //free(finger);
      //finger = bits_init(window_size);

      // update chunk_start
      chunk_start = chunk_end+1;

      }
      free(finger->byte);
      free(finger);
      free(and_temp->byte);
      free(and_temp);

      i++;
      c = fgetc(fp);

  }
  temp = add_chunk(temp, chunk_start, chunk_end);
  // debugging

  free(window->byte);
  free(window);

  // find hashes
  fp = fopen(file, "r");
  node* iter = temp->head->next;
  while (iter->next != NULL) {
    unsigned char data[iter->data->size];
    fread(data, sizeof(char), iter->data->size, fp);
    SHA1(data, iter->data->size, iter->data->hash);

    iter = iter->next;
  }

  return temp;
}

/*! Function: find_fingerprints_bytes

 Desc: fingerprints byte array with a specific window size,
       bitmask, polynomial
       min and max not yet implemented

 Pre: must pass in char array, array size, window_size, poly, bitmask

      TODO: add min and max

 Post: return a new chunks type
*/
chunks* find_fingerprints_bytes(char* bytes, int data_size, int window_size, bits* poly, bits* bitmask,int min, int max) {
  chunks* temp = init_chunks();
  temp->filename = "stdin";
  long chunk_start = 1;
  long chunk_end = 0;

 bits* window = bits_init(window_size);
 bits* finger;

  int i;

  for (i = 0; i < data_size; i++) {
    window = push_byte_window(bytes[i], window);
    finger = poly_mod(window, poly);
    chunk_end++;

    /*if (eq(and(window, bitmask), bits_initlong(0l)) &&
    	chunk_end - chunk_start > min || chunk_end - chunk_start > max) {*/
      bits* and_temp = and(finger, bitmask);

      if (eq(and_temp, bitmask)) {
      temp = add_chunk(temp, chunk_start, chunk_end);

      // reset window
      free(window->byte);
      free(window);
      window = bits_init(window_size);

      // update chunk_start
      chunk_start = chunk_end+1;
      }
      free(and_temp->byte);
      free(and_temp);
      free(finger->byte);
      free(finger);
  }
  temp = add_chunk(temp, chunk_start, chunk_end);

  free(window->byte);
  free(window);

  // find hashes
  node* iter = temp->head->next;
  while (iter->next != NULL) {
    unsigned char data[iter->data->size];
    memcpy(&data[0], &bytes[iter->data->chunk_start], iter->data->size);
    SHA1(data, iter->data->size, iter->data->hash);

    iter = iter->next;
  }

  return temp;

}

/*! Function: push_byte_window

 Desc: shifts the window 1 byte to the left using
       shiftl_byte and add byte to the right end
       of bits type

 Pre: must pass in byte and bits type



 Post: return pointer to passed in bits
*/
bits* push_byte_window(unsigned char byte, bits* window) {
  window = shiftl_byte(window, 1);
  window->byte[window->size-1] = byte;

  return window;
}

/*! Function: get_similarity

 Desc: finds the percentage of hashes that
       overlap in the two chunks out of the
       max total fingerprints

 Pre: must pass in 2 chunks data types



 Post: returns result as percentage
*/
double get_similarity(chunks* data1, chunks* data2) {
  int count1, count2, max;
  count1 = data1->fingers;
  count2 = data2->fingers;

  if (count1 > count2)
    max = count1;
  else
    max = count2;

  if (count1 == 0 && count2 == 0)
    return 1.0;

  return (double) (get_intersection_fingers(data1, data2) / max);
}

/*! Function: get_intersection_fingers

 Desc: finds the number of hashes that appear
       in both data sets


 Pre: must pass in 2 chunks data types



 Post: returns number of hashes that appear in both data sets
*/
int get_intersection_fingers(chunks* data1, chunks* data2) {
  int count = 0; // number of intersections
  node* iter1 = data1->head->next;
  node* iter2 = data2->head->next;

  // loop through both sets of fingerprints
  while (iter1->next != NULL) {
    while (iter2->next != NULL) {
      if (iter1->data->hash == iter2->data->hash)
        count++;
      iter2 = iter2->next;
    }
   iter1 = iter1->next;
  }
  return count;
}

//debug

/*! Function: print_fingerprint_test

 Desc: test fingerprinting by generating
       5 data sets 2 similar to the first
       and 2 different. the data sets are
       fingerprinted and the data is tested
       with get_similarity

 Pre: none



 Post: none
*/

void print_fingerprint_test() {

  printf("\n----------\nfingerprint tests\n----------\n");
  bits* poly = createIrreducible(16);
  // fingerprint info
  bits* bitmask= bits_initlong(0xFFF);
  int window_size = 20;
  int min = 4096;
  int max = 16384;
  chunks* data_chunk1;
  chunks* data_chunk2;
  chunks* data_chunk3;
  chunks* data_chunk4;
  chunks* data_chunk5;


  printf("Irreducible poly: %s \n", toStringHex(*poly));

  printf("Generating data sets... \n");
  //generate data set
  int data_size = (1 << 19); // 500k * 8 = 4MB
  int i;
  srand(time(0));
  char data1[data_size];
  char data2[data_size];
  char data3[data_size];
  char data4[data_size];
  char data5[data_size];

  for (i = 0; i < data_size; i++) {
    data1[i] = rand()%256;
    // different bytes
    data4[i] = rand()%256;
    data5[i] = rand()%256;
  }
  // similar bytes
  memcpy(&data2[0], &data1[0], data_size);
  memcpy(&data3[0], &data1[0], data_size);

  // change 100 bytes
  for (i = 0; i < 100; i++) {
    data2[rand()%data_size] = rand()%256;
    data3[rand()%data_size] = rand()%256;
  }

  printf("Fingerprinting data sets... \n");
  data_chunk1 = find_fingerprints_bytes(data1, data_size, window_size, poly, bitmask,min, max);
  printf("data1 done... \n");
  data_chunk2 = find_fingerprints_bytes(data2, data_size, window_size, poly, bitmask,min, max);
  printf("data2 done... \n");
  data_chunk3 = find_fingerprints_bytes(data3, data_size, window_size, poly, bitmask,min, max);
  printf("data3 done... \n");
  data_chunk4 = find_fingerprints_bytes(data4, data_size, window_size, poly, bitmask,min, max);
  printf("data4 done... \n");
  data_chunk5 = find_fingerprints_bytes(data5, data_size, window_size, poly, bitmask,min, max);
  printf("data5 done... \n");

  printf("Comparing data chunks... \n");
  printf("Similar data... \n");
  printf("data1 and data2: ");
  (abs(0.00 - get_similarity(data_chunk1, data_chunk2)) < 0.05)?printf("ok\n"):printf("failed\n");
  printf("data1 and data3: ");
  (abs(0.00 - get_similarity(data_chunk1, data_chunk3)) < 0.05)?printf("ok\n"):printf("failed\n");
  printf("Different data... \n");
  printf("data1 and data4: ");
  (abs(0.70 - get_similarity(data_chunk1, data_chunk4)) < 0.05)?printf("ok\n"):printf("failed\n");
  printf("data1 and data5: ");
  (abs(0.70 - get_similarity(data_chunk1, data_chunk5)) < 0.05)?printf("ok\n"):printf("failed\n");


  free_chunks(data_chunk1);
  free_chunks(data_chunk2);
  free_chunks(data_chunk3);
  free_chunks(data_chunk4);
  free_chunks(data_chunk5);

  free(poly->byte);
  free(poly);
  free(bitmask->byte);
  free(bitmask);
}

/*! Function: print_chunks

 Desc: prints the chunk data filename, start,
       end, offset, hash


 Pre: must pass in chunks data type



 Post: none
*/

void print_chunks(chunks* data) {
  node* iter;
  int i;
  iter = data->head->next;
  printf("\n");
  while (iter->next != NULL) {
    printf("%s: %d %d %d ",
    	   data->filename,
    	   iter->data->chunk_start,
    	   iter->data->chunk_end,
    	   iter->data->size);
    for (i = 0; i < SHA_DIGEST_LENGTH; i++)
      printf("%02x", iter->data->hash[i]);
    printf("\n");
    iter = iter->next;
  }
  //printf("%s: %d %d %d \n", data->filename, data->fingers, data->size, data->size/data->fingers);
}

/*! Function: free_chunks

 Desc: free all the allocated data with a
	 chunks data type


 Pre: must pass in chunks data type



 Post: none
*/

void free_chunks(chunks* data) {
  node *iter, *delete;
  iter = data->head->next;

  while (iter->next != NULL) {
    // current chunk
    delete = iter;
    iter = iter->next;

    free(delete->data);
    free(delete);

  }
  //
  free(data->tail);
  free(data->head);
  free(data);
}

/*! Function: print_fingerprint_report -- debug function

 Desc: fingerprints file with preset polynomial
       and prints chunk data


 Pre: must pass in filename



 Post: none
*/

void print_fingerprint_report( char*fName) {

  char temp[7]; //edit for polynomials that need generated
  temp[0] = 0x37;
  temp[1] = 0x5A;
  temp[2] = 0xD1;
  temp[3] = 0x4A;
  temp[4] = 0x67;
  temp[5] = 0xFC;
  temp[6] = 0x7B;

  bits* poly = bits_initbytes(temp, 7); //edit 7 is default in Java
  //printf("Irreducible: %d\n", getReducibility(poly));

  //bits* poly = createIrreducible(8);
  bits* bitmask= bits_initlong(0x1FFF); //edit
  chunks* data;

  printf("Irreducible poly: %s \n", toStringHex(*poly));

  // checksum: 0x1FFF
    // min: 4k
    // max: 16k
    // window 16 bytes

  //test if file exists and is usable
  //fileCheckM in main.c
  if(fileCheck(fName) == 1) {
  data = find_fingerprints_file(fName, 48, poly, bitmask, 4096, 16384);
  }
    else {
      printf("File Error\n");
  }

  print_chunks(data);
  free_chunks(data);
  free(poly->byte);
  free(poly);
  free(bitmask->byte);
  free(bitmask);
}

