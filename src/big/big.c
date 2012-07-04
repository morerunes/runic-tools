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
	FILE *in = fopen(filename, "rb");
	if (in == NULL) {
		perror("Error opening file!");
		exit(EXIT_FAILURE);
	}

	Head *header = malloc(sizeof(Head));

	//Read Header
	fread(header, 4, 4, in);

	if ((header->bigb[0] != 'B') && (header->bigb[1] != 'I') && (header->bigb[2] != 'G') && (header->bigb[3] != 'B')) {
		printf("File is not a BIGB file, exiting!");
		free(header);
		fclose(in);
		exit(EXIT_FAILURE);
	}

	if (DEBUG) {
		printHead(header);
	}

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
	for (int i = 0; i < bigfile->numBanks; i++) {
		freadNTS(bankheads[i].bankName, 254, in);
		fread(&(bankheads[i].bankID), 4, 5, in);
	}

	if (DEBUG) { // Print debug info
		for (int i = 0; i < bigfile->numBanks; i++) {
			printBankHead(&bankheads[i]);
		}
	}

	//Read Bank Data
	bigfile->banks = malloc(bigfile->numBanks * sizeof(Bank));

	//Distribute bank headers
	for (int i = 0; i < bigfile->numBanks; i++) {
		bigfile->banks[i].header = bankheads[i];
	}

	//Inner Loop Var Declarations
	INT numFiles = 0;

	for (int i = 0; i < /*bigfile->numBanks*/1; i++) { // For every bank, read bank
		fseek(in, bigfile->banks[i].header.indexOffset, SEEK_SET); // Seek to file index header
		fread(&(bigfile->banks[i].fileSet.header), 4, 1, in); // Get number of file types

		bigfile->banks[i].fileSet.header.fileTypes = malloc(
				bigfile->banks[i].fileSet.header.numFileTypes * sizeof(FileType)); // Allocate memory for file types list

		// Read file types
		for (int j = 0; j < bigfile->banks[i].fileSet.header.numFileTypes; j++) {
			fread(&(bigfile->banks[i].fileSet.header.fileTypes[j]), 4, 2, in);
			numFiles += bigfile->banks[i].fileSet.header.fileTypes[j].numFiles;
		}

		if (DEBUG) {
			printFileIndexHead(&(bigfile->banks[i].fileSet.header));
		}

		printf("1");

		// Allocate memory for files
		bigfile->banks[i].fileSet.files = malloc(sizeof(FileIndex) * numFiles);

		printf("2");

		// File index read loop
		for (int j = 0; j < 20; j++) {
			FileIndex *currentFile = &(bigfile->banks[i].fileSet.files[j]);

			// Read magic num through filename length
			fread(&(currentFile->magicNumber), 4, 7, in);

			// Read file name
			currentFile->fileName = malloc(currentFile->nameLength + 1);
			fread(currentFile->fileName, currentFile->nameLength, 1, in);
			currentFile->fileName[currentFile->nameLength] = '\0';

			// Read crc through number of source files
			fread(&(currentFile->crc), 4, 2, in);

			// Read source files info
			currentFile->devFiles = malloc(sizeof(DevFile) * currentFile->numSrcFiles);

			for (int k = 0; k < currentFile->numSrcFiles; k++) {
				DevFile *currentDevFile = &(currentFile->devFiles[k]);

				fread(&(currentDevFile->srcNameLength), 4, 1, in);

				// Read full path name
				currentDevFile->srcName = malloc(currentDevFile->srcNameLength + 1);
				fread(currentDevFile->srcName, currentDevFile->srcNameLength, 1, in);
				currentDevFile->srcName[currentDevFile->srcNameLength] = '\0';
			}

			// Read sub-header
			fread(&(currentFile->subHeaderSize), 4, 1, in);

			if (DEBUG) {
				printFileIndex(currentFile);
			}

			// Read sub-header according to dev-file type attribute
			switch (currentFile->fileTypeDev) {
			case BBMFILE:
				//TODO
//					break;
			case TGAFILE:
				//TODO
//					break;
			case BBAFILE:
				//TODO
//					break;
			default:
				printf("Unknown file type, skipping...");
				fseek(in, currentFile->subHeaderSize, SEEK_CUR);
				break;
			}
		}
	}

	return *bigfile;
}

void saveBigFile(char* filename, BigFile file) {
	//TODO: Implement big file writing
}

void destroyBigFile(BigFile *bigfile) {
	//TODO:
}
