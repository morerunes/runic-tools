/* src/debug.c
 *
 * Debugging funtions for library classes
 */
#include <stdio.h>
#include "big.h"
void printHead(Head *header) {
	printf("Header:\t\t\t");
	printf("%c%c%c%c\nVersion:\t\t%u\nBank Headers Offset:\t%u\nUnknown:\t\t%u\n", header->bigb[0], header->bigb[1],
			header->bigb[2], header->bigb[3], header->version, header->bankHeadOffset, header->unknown);
}

void printBankHead(BankHead *header) {
	printf("\nBank Name:\t%s\nBank ID:\t%u\n", header->bankName, header->bankID);
	printf("NumEntries:\t%u\nIndexStart:\t%u\nIndexSize:\t%u\n", header->numEntries, header->indexOffset,
			header->indexSize);
	printf("Block Size:\t%u\n", header->blockSize);
}

void printFileIndexHead(FileIndexHead *header) {
	printf("\nFile Index Header Info:\n");
	printf("Number of File Types:\t%u\n\n", header->numFileTypes);
	printf("File Type - (Number of Files)\n");
	int i;
	for (i = 0; i < (header->numFileTypes); i++) {
		printf("%u - (%u)\n", header->fileTypes[i].fileType, header->fileTypes[i].numFiles);
	}
}

void printFileIndex(FileIndex *index) {
	printf("\nFile Index Info:\n\n");
	printf("Magic Number:\t%u\n", index->magicNumber);
	printf("File ID:\t%u\n", index->fileID);
	printf("File Type:\t%u\n", index->fileType);
	printf("File Size:\t%u\n", index->fileSize);
	printf("File Start:\t%u\n", index->fileStart);
	printf("devFile Type:\t%u\n", index->fileTypeDev);
	printf("File Name:\t%s\n", index->fileName);
	printf("File CRC:\t%u\n", index->crc);
	printf("NumSrcFiles:\t%u\n", index->numSrcFiles);

	for (int i = 0; i < index->numSrcFiles; i++) {
		printf("devName:\t%s\n", index->devFiles[i].srcName);
	}

	printf("SubHead Size:\t%u\n", index->subHeaderSize);
}

void printMeshSHead(MeshSHead *subhead) {
	printf("\nMesh Sub-Header\n");
	printf("\nPhysMesh:\t%u\n", subhead->physicsMesh);
	printf("BBM Head:\t[ [%f, %f, %f, %f, %f]\n\t\t  [%f, %f, %f, %f, %f] ]\n", subhead->bbmHead[0],
			subhead->bbmHead[1], subhead->bbmHead[2], subhead->bbmHead[3], subhead->bbmHead[4], subhead->bbmHead[5],
			subhead->bbmHead[6], subhead->bbmHead[7], subhead->bbmHead[8], subhead->bbmHead[9]);
	printf("Num LODs:\t%u\n", subhead->numLODs);
	printf("Textures:\n");

	for (int p = 0; p < subhead->numTextures; p++) {
		printf("\t%d:\t%u\n", p, subhead->textureID[p]);
	}
}

void printTexSHead(TexSHead *subhead) {
	printf("\nTexture Sub-Header\n");
	printf("Width:\t%u\n", subhead->width);
	printf("Height:\t%u\n", subhead->height);
	printf("Depth:\t%u\n", subhead->depth);
	printf("Frame Width:\t%u\n", subhead->frameWidth);
	printf("Frame Height:\t%u\n", subhead->frameHeight);
	printf("Frame Count:\t%u\n", subhead->frameCount);
	printf("DXT Compression:\t%u\n", subhead->dxt);
	printf("Unknown:\t%u\n", subhead->unknown0);
	printf("Alpha:\t%u\n", subhead->numAlphaChannels);
	printf("MipMaps:\t%u\n", subhead->numMipMaps);
	printf("Unknown:\t%u\n", subhead->unknown1);
	printf("Top MipMap Size:\t%u\n", subhead->topMipmapSize);
	printf("Top MipMap Compressed Size:\t%u\n", subhead->topMipmapCompressedSize);
	printf("DXT Again?:\t%u\n", subhead->unknown2);
	printf("Unknown:\t%u\n", subhead->unknown3);
}
