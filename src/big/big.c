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

	for (int i = 0; i < bigfile->numBanks; i++) { // For every bank, read bank

		if (DEBUG) {
			printf("\n-- Bank #%d --\n", i + 1);
		}

		INT numFiles = 0;

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

		// Allocate memory for files
		bigfile->banks[i].fileSet.files = malloc(sizeof(FileIndex) * numFiles);

		// File index read loop
		for (int j = 0; j < numFiles; j++) {
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
			if (currentFile->subHeaderSize == 0) {
				if (DEBUG) {
					printf("No Sub-Header");
				}
			} else if (currentFile->subHeaderSize == 4) {
				printf("Dialogue not yet implemented, skipping...\n");
				fseek(in, currentFile->subHeaderSize, SEEK_CUR);
			} else if (currentFile->subHeaderSize == 24) {
				printf("Animations not yet implemented, skipping...\n");
				fseek(in, currentFile->subHeaderSize, SEEK_CUR);
			} else if (currentFile->subHeaderSize > 45) {
				//-- Read BBM Sub-Header --//
				fread(&(currentFile->subHeader.meshSHeader.physicsMesh), 4, 12, in);

				currentFile->subHeader.meshSHeader.LODSize = malloc(4 * currentFile->subHeader.meshSHeader.numLODs);
				currentFile->subHeader.meshSHeader.padding = malloc(4 * currentFile->subHeader.meshSHeader.numLODs);

				fread(&(currentFile->subHeader.meshSHeader.LODSize), 4, currentFile->subHeader.meshSHeader.numLODs, in);
				fread(&(currentFile->subHeader.meshSHeader.padding), 4, currentFile->subHeader.meshSHeader.numLODs, in);
				fread(&(currentFile->subHeader.meshSHeader.numTextures), 4, 1, in);

				currentFile->subHeader.meshSHeader.textureID = malloc(
						4 * currentFile->subHeader.meshSHeader.numTextures);
				fread(currentFile->subHeader.meshSHeader.textureID, 4, currentFile->subHeader.meshSHeader.numTextures,
						in);

				if (DEBUG) {
					printMeshSHead(&(currentFile->subHeader.meshSHeader));
				}
			} else if (currentFile->subHeaderSize == 34) {
				printf("Textures not yet implemented, skipping...\n");
				fseek(in, currentFile->subHeaderSize, SEEK_CUR);
			} else {
				printf("Unknown file type, skipping...");
				fseek(in, currentFile->subHeaderSize, SEEK_CUR);
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
