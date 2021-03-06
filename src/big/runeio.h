/* src/runeio.h
 *
 * header file with function prototypes
 */

#ifndef RUNEIO_H_INCLUDED
#define RUNEIO_H_INCLUDED
#include "big.h"
void freadNTS(char *str, int num, FILE *stream);
short fensureDir(char *filename);
FILE* fsafeOpenForWrite(char *filename, short overwrite);
#endif // RUNEIO_H_INCLUDED
