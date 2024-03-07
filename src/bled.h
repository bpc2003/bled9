#include <stdio.h>
#define END -1

// file operations
char **readfile(char *filename);
void writefile(char *filename, char **buf);
char **readtmp(FILE *tmp);
void writetmp(FILE *tmp, char **buf);

// buffer operations
void printbuf(char **buf, int sl, int start, int end);
void insertbuf(char **buf, int pos, int ow, int ins);
void dellines(char **buf, int start, int end);
void undo(FILE *tmp, char **buf);
