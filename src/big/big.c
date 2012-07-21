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

short DEBUG = 0;

BigFile* readBigFile(char* filename) {
	BigFile *bigfile = malloc(sizeof(BigFile));
	Head *header = &(bigfile->header);

	bigfile->filename = filename;

	//-- Begin Reading File --//
	FILE *in = fopen(filename, "rb");
	bigfile->file = in;
	if (in == NULL ) {
		perror("Error opening file!");
		exit(EXIT_FAILURE);
	}

	//Read Header
	fread(header, 4, 4, in);

	if ((header->bigb[0] != 'B') && (header->bigb[1] != 'I') && (header->bigb[2] != 'G') && (header->bigb[3] != 'B')) {
		printf("File is not a BIGB file, exiting!");
		free(bigfile);
		printf("Closing File...\n");
		fclose(in);
		exit(EXIT_FAILURE);
	}

	if (DEBUG > 1) {
		printHead(header);
	}

	//Get Num Bank Headers
	INT numBanks;
	fseek(in, bigfile->header.bankHeadOffset, SEEK_SET);
	fread(&numBanks, 4, 1, in);
	bigfile->numBanks = numBanks;

	if (DEBUG > 1) {
		printf("\nThere are %d banks.\n", bigfile->numBanks);
	}

	BankHead *bankheads = malloc(bigfile->numBanks * sizeof(BankHead));

	//Read Bank Headers
	for (int i = 0; i < bigfile->numBanks; i++) {
		freadNTS(bankheads[i].bankName, 254, in);
		fread(&(bankheads[i].bankID), 4, 5, in);
	}

	if (DEBUG > 1) { // Print debug info
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

		if (DEBUG > 1) {
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

		if (DEBUG > 1) {
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

			if (DEBUG > 1) {
				printFileIndex(currentFile);
			}

			// Read sub-header according to dev-file type attribute
			if (currentFile->subHeaderSize == 0) {
				if (DEBUG > 1) {
					printf("No Sub-Header");
				}
			} else if (currentFile->subHeaderSize == 4) {
				if (DEBUG > 0) {
					printf("Dialogue not yet implemented, skipping...\n");
				}
				fseek(in, currentFile->subHeaderSize, SEEK_CUR);
			} else if (currentFile->subHeaderSize == 24) {
				if (DEBUG > 0) {
					printf("Animations not yet implemented, skipping...\n");
				}
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

				if (DEBUG > 1) {
					printMeshSHead(&(currentFile->subHeader.meshSHeader));
				}
			} else if (currentFile->subHeaderSize == 34) {
				//-- Read DDS Subhead --//
				fread(&(currentFile->subHeader.meshSHeader), 2, 17, in);

				if (DEBUG > 1) {
					printTexSHead(&(currentFile->subHeader.texSHeader));
				}
			} else {
				if (DEBUG > 0) {
					printf("Unknown file type, skipping...");
				}
				fseek(in, currentFile->subHeaderSize, SEEK_CUR);
			}
		}
	}

	return bigfile;
}

/* exportFileIndex(BigFile *big, INT bank, INT fileID, short replace)
 *
 * This function will export the file with the id specified from the bank specified
 * No conversion will take place, it will simply export the binary format present in the big file
 *
 * Returns 0 if exported correctly, 1 if file already existed and replace == 0, 2 if there was another IO error
 */

short exportFileIndex(BigFile *big, INT bank, INT fileID, short replace) {
	//-- Read Data --//
//	FILE *in = fopen(big->filename, "rb");
	FILE *in = big->file;

	if (in == NULL ) {
		printf("File didn't exist, retrying...\n");
		in = fopen(big->filename, "rb");
	}

	if (in == NULL ) {
		perror("Error opening file!");
		exit(EXIT_FAILURE);
	}

	int offset = big->banks[bank].fileSet.files[fileID - 1].fileStart;
	int size = big->banks[bank].fileSet.files[fileID - 1].fileSize;
	char *pathname = "output";

	char *outputFileName;
	if (big->banks[bank].fileSet.files[fileID - 1].numSrcFiles != 0) {
		outputFileName = calloc(
			strlen(pathname) + strlen(big->banks[bank].fileSet.files[fileID - 1].devFiles[0].srcName) + 1, 1);
		strcpy(outputFileName, pathname);
		strcat(outputFileName, big->banks[bank].fileSet.files[fileID - 1].devFiles[0].srcName);
	} else {
		outputFileName = calloc(
			strlen(pathname) + strlen(big->banks[bank].fileSet.files[fileID - 1].fileName) + 2, 1);
			strcpy(outputFileName, pathname);
			strcat(outputFileName, "\\");
			strcat(outputFileName, big->banks[bank].fileSet.files[fileID - 1].fileName);
	}

	char *buffer = calloc(size, 1);

	fseek(in, offset, SEEK_SET);
	fread(buffer, size, 1, in);

//	//-- Convert Pathname --//
//	for (int i = 0; i < strlen(outputFileName); i++) {
//		if (outputFileName[i] == '\\') {
//			outputFileName[i] = '/';
//		}
//	}

	//-- Detect File Type --//
	int isText = 0;

	if (big->banks[bank].fileSet.files[fileID - 1].subHeaderSize == 4) {
		// This is probably dialogue or text, assume this is text for now
		// This means you should not try to export dialogue.big!
		// DON'T DO IT

		isText = 1;
	}

	//-- Detect if it is a 'group' text object --//
	if (big->banks[bank].fileSet.files[fileID - 1].fileType == 1) {
		free(outputFileName);

		return 0;
	}

	//-- Write Data --//
	fensureDir(outputFileName);
	FILE *out = fopen(outputFileName, "rb");

	if (out != NULL ) {
		if (replace) {
			//printf("Output file exists, overwriting...\n");
		} else {
			printf("The output file already exists, skipping...\n");
			free(outputFileName);
			fclose(out);
			return 0;
		}
	}

	fclose(out);

	if (!isText) {
		out = fopen(outputFileName, "wb");

		if (out == NULL ) {
			printf("IO Error! Cannot open new file for writing!\n");
			printf("%s\n", outputFileName);
			return 2;
		}

		if (fwrite(buffer, 1, size, out) != size) {
			printf("Error writing file!\n");
			free(buffer);
			free(outputFileName);
			fclose(out);
			rewind(in);
			return 2;
		}

		free(buffer);
		free(outputFileName);
		fclose(out);

		rewind(in);

		return 0;
	} else {
		out = fopen(outputFileName, "wt");

		if (out == NULL ) {
			printf("IO Error! Cannot open new file for writing!\n");
			printf("%s\n", outputFileName);
			return 2;
		}

		//-- Detect if there is text to output --//
		if (buffer[0] == 32) {
			// There's no text in this one
			free(buffer);
			free(outputFileName);
			fclose(out);

			rewind(in);

			return 0;
		}

		for (int i = 0; i < size; i++) {
			if (buffer[i] == '\0' && buffer[i+1] == '\0') {
				if (fwrite(buffer, 1, i+1, out) != i+1) {
					printf("Error writing file!\n");
					free(buffer);
					free(outputFileName);
					fclose(out);
					rewind(in);
					return 2;
				}

				break;
			}
		}

		free(buffer);
		free(outputFileName);
		fclose(out);

		rewind(in);

		return 0;
	}
}

short exportBank(BigFile *bigFile, short bank, short replace) {
	if (bank > (bigFile->numBanks - 1) || bank < 0) {
		printf("Invalid Bank Specified!\n");
		return 1;
	} else {
		int numFiles = bigFile->banks[bank].header.numEntries;
		for (int i = 1; i <= numFiles; i++) {
			exportFileIndex(bigFile, bank, i, replace);
		}

		return 0;
	}
}

void saveBigFile(char* filename, BigFile file) {
	//TODO: Implement big file writing
}

void destroyBigFile(BigFile *bigfile) {
	//TODO:
}
