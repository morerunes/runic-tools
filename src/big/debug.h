/* src/debug.h
 *
 * Header for debug functions
 * Contains function prototypes
 */

#ifndef RUNE_DEBUG_H
#define RUNE_DEBUG_H
#include "big.h"
void printHead(Head *header);
void printBankHead(BankHead *header);
void printFileIndexHead(FileIndexHead *header);
void printFileIndex(FileIndex *index);
void printMeshSHead(MeshSHead *subhead);
void printTexSHead(TexSHead *subhead);
#endif //RUNE_DEBUG_H
