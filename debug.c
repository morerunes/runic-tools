/* src/debug.c
 *
 * Debugging funtions for library classes
 */
#include <stdio.h>
#include "big.h"
void printHead(Head *header) {
	printf("Header:\t\t\t");
	printf("%c%c%c%c\nVersion:\t\t%u\nBank Headers Offset:\t%u\nUnknown:\t\t%u\n",
		header->bigb[0], header->bigb[1], header->bigb[2], header->bigb[3],
		header->version, header->bankHeadOffset, header->unknown);
}

void printBankHead(BankHead *header) {
	printf("\nBank Name:\t%s\nBank ID:\t%u\n", header->bankName, header->bankID);
	printf("NumEntries:\t%u\nIndexStart:\t%u\nIndexSize:\t%u\n",
			header->numEntries, header->indexOffset, header->indexSize);
	printf("Block Size:\t%u\n", header->blockSize);
}

void printFileIndexHead(FileIndexHead *header) {
	printf("\nFile Index Header Info:\n\n");
	printf("Number of File Types:\t%d\n", header->numFileTypes);
	printf("File Type:\t\t%d\n", header->fileType);
	printf("Number of Files:\t%d\n", header->numFiles);
}
