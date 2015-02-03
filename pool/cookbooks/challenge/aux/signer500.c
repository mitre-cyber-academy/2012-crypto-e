/*
 * signer500.c
 *
 *  Created on: Jun 21, 2012
 *      Author: MLISKOV
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>

int unhex(char h) {
	if ('0' <= h && h <= '9') return (h - '0');
	else if ('a' <= h && h <= 'f') return (10+h-'a');
	else return (10+h-'A');
}

int main (int argc, char ** argv) {

	if (argc != 4) {
		printf ("Usage: %s modulus d input\n", argv[0]);
		return 0;
	}

	/* FILE *skfile = fopen (argv[1], "rb"); */
	FILE *Nfile = fopen (argv[1], "r");
	FILE *dfile = fopen (argv[2], "r");
	FILE *infile = fopen (argv[3], "rb");

	// Read in the private key
	mpz_t N;
	mpz_t d;

	mpz_init(N);
	mpz_init(d);
	mpz_inp_str(N,Nfile,16);
	mpz_inp_str(d,dfile,16);

	mpz_t tmp;
	mpz_init (tmp);

	size_t siglen = 1024;
	unsigned char forbidden[siglen];
	memset(forbidden, 0, siglen);
	// What is forbidden is "FlagMe" as the last 6 characters with all preceding digits = 0.
	forbidden[siglen-6] = 'F';
	forbidden[siglen-5] = 'l';
	forbidden[siglen-4] = 'a';
	forbidden[siglen-3] = 'g';
	forbidden[siglen-2] = 'M';
	forbidden[siglen-1] = 'e';

	unsigned char input[siglen];
	// Retrieve the input
	memset(input, 0, siglen);
	// Get the size
	size_t insize = 0;
	fseek(infile, 0L, SEEK_END);
	insize = ftell(infile);
	fseek(infile, 0L, SEEK_SET);
	// If insize is bigger than siglen, refuse.
	if (insize > siglen) {
		printf ("Error: Input too long.\n");
		return 0;
	}

	if (fread(&input[siglen-insize], sizeof(char), insize, infile) < insize) {
		// Should never happen
		printf ("Error: Input too short.\n");
		return 0;
	}

//	printf ("Read input.\n");


	// Check if the input is equal to the forbidden input
	if (!memcmp(input, forbidden, siglen)) {
		printf ("Request denied.\n");
		return 0;
	}

	// Convert input to mpz.
	mpz_t input_mpz;
	mpz_init (input_mpz);
	for (int i = 0; i < siglen; i++) {
		mpz_mul_ui(tmp, input_mpz, 256);
		mpz_add_ui(input_mpz, tmp, input[i]);
	}

	// Calculate the signature
	unsigned char signature[siglen];
	memset(signature, 0, siglen);

	// tmp <- m^d mod N.
	mpz_powm(tmp, input_mpz, d, N);

//	gmp_printf("Calculated signature: %Zd\n", tmp);

	// Convert mpz to character string
	mpz_out_str(stdout, 16, tmp);
}
