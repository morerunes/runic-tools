/* src/big.c
 *
 * library for reading and writing
 * Fable TLC big files
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "big.h"
#include "debug.h"
#include "runeio.h"

short DEBUG = 1;

BigFile readBigFile(char* filename) {
	//-- Begin Reading File --//
	FILE *in = fopen(filename, "r");
	if (in == NULL) {
		perror("Error opening file!");
		exit(EXIT_FAILURE);
	}

	Head *header = malloc(sizeof(Head));

	//Read Header
	fread(header, 4, 4, in);
	if (DEBUG) { printHead(header); }

	BigFile *bigfile = malloc(sizeof(BigFile));
	bigfile->header = *header;

	//Get Num Bank Headers
	INT numBanks;
	fseek(in, bigfile->header.bankHeadOffset, SEEK_SET);
	fread(&numBanks, 4, 1, in);
	bigfile->numBanks = numBanks;

	if (DEBUG) {
		printf("\nThere are %d banks.\n", bigfile->numBanks);
	}

	BankHead *bankheads = malloc(bigfile->numBanks * sizeof(BankHead));

	//Read Bank Headers
	int i = 0;
	while (i < bigfile->numBanks) {
		freadNTS(bankheads[i].bankName, 254, in);
		fread(&(bankheads[i].bankID), 4, 5, in);
		i++;
	}

	if (DEBUG) { // Print debug info
		for (i = 0; i < bigfile->numBanks; i++) {
			printBankHead(&bankheads[i]);
		}
	}

	//Read Bank Data
	bigfile->banks = malloc(bigfile->numBanks * sizeof(Bank));

	//Distribute bank headers
	for (i = 0; i < bigfile->numBanks; i++) {
		bigfile->banks[i].header = bankheads[i];
	}

	for (i = 0; i < /*bigfile->numBanks*/1; i++) { // For every bank, read bank
		fseek(in, bigfile->banks[i].header.indexOffset, SEEK_SET); // Seek to file index header
		fread(&(bigfile->banks[i].fileSet.header), 4, 3, in); // Read file index header
		if (DEBUG) { printFileIndexHead(&(bigfile->banks[i].fileSet.header)); }
	}

	return *bigfile;
}

void saveBigFile(char* filename, BigFile file) {
	//TODO: Implement big file writing
}

void destroyBigFile(BigFile *bigfile) {
	//TODO:
}
