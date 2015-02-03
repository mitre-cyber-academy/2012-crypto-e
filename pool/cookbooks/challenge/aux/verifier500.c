/*
 * decryptor500.c
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
		printf ("Usage: %s modulus e input\n", argv[0]);
		return 0;
	}

	/* FILE *skfile = fopen (argv[1], "rb"); */
	FILE *Nfile = fopen (argv[1], "r");
	FILE *efile = fopen (argv[2], "r");
	FILE *infile = fopen (argv[3], "rb");

	// Read in the private key
	mpz_t N;
	mpz_t e;

	mpz_init(N);
	mpz_init(e);
	mpz_inp_str(N,Nfile,16);
	mpz_inp_str(e,efile,16);

	size_t siglen = 1024;
	unsigned char flagme[siglen];
	memset(flagme, 0, siglen);
	// "FlagMe" as the last 6 characters with all preceding digits = 0.
	flagme[siglen-6] = 'F';
	flagme[siglen-5] = 'l';
	flagme[siglen-4] = 'a';
	flagme[siglen-3] = 'g';
	flagme[siglen-2] = 'M';
	flagme[siglen-1] = 'e';

	mpz_t tmp;
	mpz_init(tmp);
	// Convert input to mpz.
	mpz_t flagme_mpz;
	mpz_init (flagme_mpz);
	for (int i = 0; i < siglen; i++) {
		mpz_mul_ui(tmp, flagme_mpz, 256);
		mpz_add_ui(flagme_mpz, tmp, flagme[i]);
	}

	// Get the signature from the input file
	mpz_t sig;
	mpz_init(sig);
	mpz_inp_str(sig,infile,16);

	// Calculate sig^e mod N
	mpz_powm(tmp, sig, e, N);
	// correct signature
	if (!mpz_cmp(tmp, flagme_mpz)) {
		printf ("MCA-5C8AEC67\n");
	} else
		printf ("Not a valid signature of \"FlagMe\".  Request denied.\n");

}
