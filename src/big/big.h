/* src/big.h
 *
 * library header for reading and writing
 * Fable TLC big files
 */

#ifndef BIG_H
#define BIG_H

#include <stdint.h>

//-- Type Defs --//
// primitive aliases
typedef uint32_t INT;
typedef uint16_t SHORT;
typedef uint8_t CHAR;

typedef struct {
	CHAR bigb[4];
	INT version;
	INT bankHeadOffset;
	INT unknown;
} Head;

typedef struct {
	CHAR bankName[255];
	INT bankID;
	INT numEntries;
	INT indexOffset;
	INT indexSize;
	INT blockSize;
} BankHead;

typedef struct {
	INT fileType;
	INT numFiles;
} FileType;

typedef struct {
	INT numFileTypes;
	FileType *fileTypes;
} FileIndexHead;

typedef struct {
	INT physicsMesh;
	float bbmHead[10];
	INT numLODs;
	INT *LODSize;
	INT *padding;
	INT numTextures;
	INT *textureID;
} MeshSHead;

typedef struct {
	SHORT width;
	SHORT height;
	SHORT depth;
	SHORT frameWidth;
	SHORT frameHeight;
	SHORT frameCount;
	SHORT dxt;
	SHORT unknown0;
	CHAR numAlphaChannels;
	CHAR numMipMaps;
	SHORT unknown1;
	INT topMipmapSize;
	INT topMipmapCompressedSize;
	SHORT unknown2;
	INT unknown3;
} TexSHead;

typedef union {
	MeshSHead meshSHeader;
	TexSHead texSHeader;
} Subhead;

typedef struct {
	INT srcNameLength;
	CHAR* srcName;
} DevFile;

typedef struct {
	INT magicNumber;
	INT fileID;
	INT fileType;
	INT fileSize;
	INT fileStart;

	/* DEV INFO */
	INT fileTypeDev;
	INT nameLength;
	CHAR* fileName;
	INT crc;
	INT numSrcFiles;
	DevFile* devFiles;

	/* END DEV INFO */

	INT subHeaderSize;
	Subhead subHeader; //union
} FileIndex;

/* NOTE:
 *
 * The name FileIndex is being used for two reasons:
 * First, FileIndex was the term used at the FableTLCMod wiki file format doc.
 *
 * Second, I do not want there to be any confusion between the FILE type part
 * 	of stdio.h and a file within the big file.
 */

typedef struct {
	FileIndexHead header;
	FileIndex* files;
} FileSet;

typedef struct {
	BankHead header;
	FileSet fileSet;
} Bank;

typedef struct {
	Head header;

	INT numBanks;
	Bank* banks;
} BigFile;

//-- Function Prototypes --//
BigFile readBigFile(char* filename);
void saveBigFile(char* filename, BigFile file);
void destroyBigFile(BigFile *bigfile);

#endif // BIG_H
