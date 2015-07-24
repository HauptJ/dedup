
#ifndef __FINGERPRINT__
#define __FINGERPRINT__


//   File: fingerprint.h
//   Name: Joshua Haupt, Nick Kirkpatrick, Spencer Smith
//   Date: Spring 15
// Course: CS 495 - Crk Group
//   Desc: This file includes functions declarations and data structures for fingerprinting. 
//         

#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include "bit_array.h"


// read java files: fingerprintpoly.java 

typedef struct node node;

// Stores info about fingerprint chunk
typedef struct {
  unsigned char hash[SHA_DIGEST_LENGTH];
  int chunk_start;
  int chunk_end;
  int size;
} chunk;

// Linked list node to store chunks

struct node {
  node* previous;
  node* next;
  chunk* data;
};

// Stores all fingerprint chunks and info 
typedef struct {
  node* head;
  node* tail;
  char* filename;
  int fingers;
  int size;
} chunks; 

//chunks
chunks* init_chunks();
node* init_node(node*, chunk*, node*);
chunks* add_chunk(chunks*, int, int);
void free_chunks(chunks*);

//file check
int fileCheck(char *fname);

//fingerprint 

chunks* find_fingerprints_file(char*, int, bits*, bits*, int, int);
chunks* find_fingerprints_bytes(char*, int, int, bits*, bits*, int, int);
bits* push_byte_window(unsigned char,bits*);

//fingerprint testing
double get_similarity(chunks*, chunks*);
int get_intersection_fingers(chunks*, chunks*);

//debug
void print_fingerprint_test();
void print_chunks(chunks*);
void print_fingerprint_report();

#endif