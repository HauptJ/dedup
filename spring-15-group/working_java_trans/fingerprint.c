
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


//file check
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


<<<<<<< .mine
byte_queue add_byte(unsigned char byte, byte_queue window) {

    window.data[window.head] = byte;
    window.head = (window.head+1)%(window.capacity);
    window.size++;

  return window;
}

byte_queue poll_byte(byte_queue window) {
   window.tail = (window.tail+1)%(window.capacity);
   window.size--;
   return window;
}


=======
>>>>>>> .r101
//chunks
chunks init_chunks() {
  chunks temp;
  temp.size = 0;
  temp.fingers = 0;

  temp.head = init_node(NULL, NULL, NULL);
  temp.tail = init_node(temp.head, NULL, NULL);
  temp.head->next = temp.tail;

  return temp;
}

node* init_node(node* previous, chunk* data, node* next) {
  node* temp = malloc(sizeof(node));
  temp->previous = previous;
  temp->data = data;
  temp->next = next;

  return temp;
}

chunks add_chunk(chunks data, bits finger, int start, int end) {
  chunk* temp_chunk = malloc(sizeof(chunk));
  temp_chunk->fingerprint = finger;
  temp_chunk->chunk_start = start;
  temp_chunk->chunk_end = end;
<<<<<<< .mine

=======
  temp_chunk->size = end - start;
 
>>>>>>> .r101
  node* temp = init_node(data.tail->previous, temp_chunk, data.tail);
  data.tail->previous->next = temp;
  data.tail->previous = temp;
  data.size += (end-start);
  data.fingers++;
  return data;
}

//fingerprint
chunks find_fingerprints_binary_file(char* file, int window_size, bits poly, bits checksum, int min, int max) {
  chunks temp = init_chunks();
  FILE *fp;
  fp = fopen(file, "r");
  long chunk_start = 0;
  long chunk_end = 0;

  bits window = bits_init();
  bits finger = bits_init();

  char c;

  while((c = fgetc(fp)) != EOF) {
    window = push_byte(c, window, poly);
    finger = push_byte(c, finger, poly);
    chunk_end++;
<<<<<<< .mine

    if (eq(and(window.bits, checksum), bits_initlong(0l)) &&
=======
    
    if (eq(and(window, checksum), bits_initlong(0l)) && 
>>>>>>> .r101
    	chunk_end - chunk_start > min || chunk_end - chunk_start > max) {

      temp = add_chunk(temp, finger, chunk_start, chunk_end);
      // debugging

      // reset finger
      finger = bits_init();
      // update chunk_start
      chunk_start = chunk_end;
    }
  }
  temp = add_chunk(temp, finger, chunk_start, chunk_end);
  // debugging


  return temp;
}
chunks find_fingerprints_file(char* file, int window_size, bits poly, bits checksum, int min, int max) {
  chunks temp = init_chunks();
  FILE *fp;
  fp = fopen(file, "r");
  long chunk_start = 0;
  long chunk_end = 0;
 
  bits window = bits_init();
  bits finger = bits_init();
 
  char c;
  
  while((c = fgetc(fp)) != EOF) {
    window = push_byte(c, window, poly);
    finger = push_byte(c, finger, poly);
    chunk_end++;
    
    /*if (eq(and(window, checksum), bits_initlong(0l)) && 
    	chunk_end - chunk_start > min || chunk_end - chunk_start > max) {*/
    if (eq(and(window, checksum), bits_initlong(0l))) {
      
      temp = add_chunk(temp, finger, chunk_start, chunk_end);
      // debugging
      
      finger = bits_init();
      // update chunk_start
      chunk_start = chunk_end;
    }
  }
  temp = add_chunk(temp, finger, chunk_start, chunk_end);
  // debugging
  	     
  
  return temp;
}

char* file_to_bytes(char*fileName){ // shoves a file into array suitable for use with find_fingerprints_bytes
      // address to current file
    FILE *file;

    unsigned long fSize;
    file=fopen(fileName,"rb");

    //get file size
    fseek(file,0,SEEK_END);
    fSize= ftell(file);
    rewind(file);

    //shove data into an array
    char* bArr = malloc(fSize);
    fread(bArr,1, fSize,file);

    fclose(file);
    return bArr;
}
chunks find_fingerprints_bytes(char* bytes, int data_size, int window_size, bits poly, bits checksum, int min, int max) {
  chunks temp = init_chunks();
  long chunk_start = 0;
  long chunk_end = 0;

 bits window = bits_init();
 bits finger = bits_init();

  int i;

  for (i = 0; i < data_size; i++) {
    window = push_byte(bytes[i], window, poly);
    finger = push_byte(bytes[i], finger, poly);
    chunk_end++;
<<<<<<< .mine

    if (eq(and(window.bits, checksum), bits_initlong(0l)) &&
      chunk_end - chunk_start > min || chunk_end - chunk_start > max) {

=======
    
    if (eq(and(window, checksum), bits_initlong(0l)) && 
    	chunk_end - chunk_start > min || chunk_end - chunk_start > max) {
      
>>>>>>> .r101
      temp = add_chunk(temp, finger, chunk_start, chunk_end);

      // reset finger
      finger = bits_init();
      // update chunk_start
      chunk_start = chunk_end;
    }
  }
<<<<<<< .mine
  temp = add_chunk(temp, finger.bits, chunk_start, chunk_end);

  free(finger.data);
  free(window.data);
=======
  temp = add_chunk(temp, finger, chunk_start, chunk_end);

>>>>>>> .r101
  return temp;

}

<<<<<<< .mine

byte_queue push_byte(unsigned char byte, byte_queue window, bits poly) {
  bits temp1 = bits_initbits(window.bits);
  temp1 = shiftl(temp1, 8);
  temp1 = or(temp1, (bits_initlong(byte & 0xFF)));
  temp1 = poly_mod(temp1, poly);
  window.bits = temp1;
=======
bits push_byte(unsigned char byte, bits window, bits poly) {
  window = shiftl(window, 8);
  window = or(window, (bits_initlong(byte & 0xFF)));
  window = poly_mod(window, poly);
>>>>>>> .r101

  return window;
<<<<<<< .mine

=======
>>>>>>> .r101
}


double get_similarity(chunks data1, chunks data2) {
  int count1, count2, max;
  count1 = data1.fingers;
  count2 = data2.fingers;

  if (count1 > count2)
    max = count1;	
  else
    max = count2;

  if (count1 == 0 && count2 == 0)
    return 1.0;

  return (double) (get_intersection_fingers(data1, data2) / max);
}

int get_intersection_fingers(chunks data1, chunks data2) {
  int count = 0; // number of intersections
  node* iter1 = data1.head->next;
  node* iter2 = data2.head->next;

  // loop through both sets of fingerprints
  while (iter1->next != NULL) {
    while (iter2->next != NULL) {
      if (eq(iter1->data->fingerprint, iter2->data->fingerprint))
        count++;
      iter2 = iter2->next;
    }
   iter1 = iter1->next;
  }
  return count;
}

//debug
void print_fingerprint_test() {

  printf("\n----------\nfingerprint tests\n----------\n");
  bits poly = createIrreducible(BYTES-1);
  // fingerprint info
  bits checksum = bits_initlong(0x1FFF);
  int window_size = 32;
  int min = 4096;
  int max = 16384;
  chunks data_chunk1 = init_chunks();
  chunks data_chunk2 = init_chunks();
  chunks data_chunk3 = init_chunks();
  chunks data_chunk4 = init_chunks();
  chunks data_chunk5 = init_chunks();


  printf("Irreducible poly: %s \n", toStringHex(poly));

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
  memcpy(data2, data1, data_size/8);
  memcpy(data3, data1, data_size/8);

  // change 100 bytes
  for (i = 0; i < 100; i++) {
    data2[rand()%data_size] = rand()%256;
    data3[rand()%data_size] = rand()%256;
  }

  printf("Fingerprinting data sets... \n");
  data_chunk1 = find_fingerprints_bytes(data1, data_size, window_size, poly, checksum, min, max);
  printf("data1 done... \n");
  data_chunk2 = find_fingerprints_bytes(data2, data_size, window_size, poly, checksum, min, max);
  printf("data2 done... \n");
  data_chunk3 = find_fingerprints_bytes(data3, data_size, window_size, poly, checksum, min, max);
  printf("data3 done... \n");
  data_chunk4 = find_fingerprints_bytes(data4, data_size, window_size, poly, checksum, min, max);
  printf("data4 done... \n");
  data_chunk5 = find_fingerprints_bytes(data5, data_size, window_size, poly, checksum, min, max);
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

/*
  free_chunks(data_chunk1);
  free_chunks(data_chunk2);
  free_chunks(data_chunk3);
  free_chunks(data_chunk4);
  free_chunks(data_chunk5);
*/
}

void print_chunks(chunks data) {
  node* iter;
  iter = data.head->next;

  /*
  printf("\n\n");
  while (iter->next != NULL) {
    printf("fingerprint: %s \n start: %d \n end: %d \n size: %d \n",
    	   toStringHex(iter->data->fingerprint), iter->data->chunk_start,
    	   iter->data->chunk_end, iter->data->size);
    iter = iter->next;
  }
  */
  printf("fingers: %d \n total size: %d \n average size: %d \n", data.fingers, data.size, data.size/data.fingers);
}

void free_chunks(chunks data) {
  node *iter, *delete;
  delete = data.head;
  iter = data.head->next;
  
  while (iter->next != NULL) {
    // previous node
    free(delete);
    // current chunk
    free(iter->data);
    delete = iter;
    iter = iter->next;
  }
  // 
  free(data.tail);
}

void print_window_report() {
  srand(time(0));
  int window_size = 8;
  int i, j;
  unsigned char data[window_size * 10];
  for (i = 0; i < (window_size * 10); i++)
    data[i] = rand()%256;

  printf("\n----------\nwindow tests\n----------\n");
  bits poly = createIrreducible(15);
  printf("Irreducible poly: %s \n", toStringHex(poly));

  bits rabin0 = bits_init();
  bits rabin1 = bits_init();


  for (j = 0; j < window_size * 3; j++) {
    rabin0 = push_byte(data[j], rabin0, poly);
  }

  for (j = window_size * 3; j < window_size * 4; j++) {
    rabin0 = push_byte(data[j], rabin0, poly);
    rabin1 = push_byte(data[j], rabin1, poly);
  }


  printf("window: ");
  (eq(rabin0, rabin1))?printf("ok\n"):printf("failed\n");


}

void print_fingerprint_report( char *fName) { //takes in input for file to fingerprint

<<<<<<< .mine
  bits poly = createIrreducible(BYTES-1);
  bits checksum = bits_initlong(0x1FFF);
=======


  bits poly = createIrreducible(8); //edit
  bits checksum = bits_initlong(0x1FFF); //edit	
>>>>>>> .r101
  chunks data;

  printf("Irreducible poly: %s \n", toStringHex(poly));
<<<<<<< .mine

  // checksum: 0x1FFF
=======
  printf("Checksum: %s \n", toStringHex(checksum));
  // checksum: 0x1FFF 
>>>>>>> .r101
    // min: 4k
    // max: 16k
    // window 16 bytes

  //test if file exists and is usable
  printf(fName);
  if(fileCheck(fName) == 1) {
  data = find_fingerprints_file(fName, 16, poly, checksum, 4096, 16384); //last 2 boundaries
  }
    else {
      printf("File Error\n");
  }
  print_chunks(data);
}

