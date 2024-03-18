#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "bled.h"

FILE *tmp;
char *shcmd;
char *filename;
char **buf;
char *pat;
char *rep;

void command(char, int, int, int);

int main(int argc, char **argv)
{
	if(argc > 2)
		filename = argv[1];
	tmp = tmpfile();
	
	shcmd = (char *)calloc(99, sizeof(char));
	char *cmdstr = NULL;
	size_t size = 0;
	
	pat = (char *)calloc(100, sizeof(char));
	rep = (char *)calloc(100, sizeof(char));
	int *lines, lineslen = 0;

	buf = readfile(filename);
	if (buf == NULL)
		return 1;

	int start = 1;
	int end = END;
	do {
		getline(&cmdstr, &size, stdin);
		char cmd;

		int dst = 0;
		int i = 0;

		while (!isalpha(cmdstr[i]) && cmdstr[i] != '!') {
			if (isdigit(cmdstr[i])) {
				for (end = 0; isdigit(cmdstr[i]); i++)
					end = 10 * end + (cmdstr[i] - '0');
				start = end;
				if (cmdstr[i] == ',')
					i++;
				else
					break;
				if (cmdstr[i] == '$') {
					end = END;
					i++;
				} else {
					for (end = 0; isdigit(cmdstr[i]); i++)
						end =
						    10 * end + (cmdstr[i] -
								'0');
				}
			} else if (cmdstr[i] == ',') {
				start = 1;
				end = END;
				i++;
			} else {
				break;
			}
		}

		cmd = cmdstr[i];
		if (!isalpha(cmd) && cmd != '!')
			continue;
		char *str;
		if (cmdstr[0] == '!')
			str = shcmd;
		else if (cmdstr[1] == '/' || cmdstr[1] == '?' || cmdstr[1] == '\\')
			str = pat; 
		if (cmdstr[i + 1]) {
			while (cmdstr[i]) {
				if (isdigit(cmdstr[i]))
					dst = 10 * dst + (cmdstr[i] - '0');
				else if (cmdstr[0] == '!') {
					if (i < 98 && cmdstr[i])
						*str++ = cmdstr[i+1];
				} else if (cmdstr[i] == '/' || cmdstr[i] == '\\' || cmdstr[i] == '?') {
					char t = cmdstr[i];
					if (i > 1) {
						t = '\n';
						str = rep;
					}
					while(cmdstr[++i] != t)
						*str++ = cmdstr[i];
					continue;
				} else if (cmdstr[i] == ' ') {
					cmdstr[strlen(cmdstr) - 1] = '\0';
					filename = strdup(cmdstr + i + 1);
				}
				++i;
			}
		}

		if (cmd == 'g') {
			lines = find(buf, pat, &lineslen);
			cmd = cmdstr[i - 2];
			if(lineslen == 0)
				continue;
		} else if (cmd == 'e') {
			for (int i = 0; buf[i]; ++i)
				free(buf[i]);
			free(buf);
			buf = readfile(filename);
			continue;
		}
		
		if (lineslen > 0) {
			int *it = lines;
			while (lineslen > 0) {
				command(cmd, *it, *it, dst);
				*it++;
				lineslen--;
			}
			free(lines);
			continue;
		}

		command(cmd, start, end, dst);	
	} while(strcmp(cmdstr, "q\n"));

	fclose(tmp);
	for (int i = 0; buf[i]; ++i)
		free(buf[i]);
	free(buf);
	free(pat);
	free(rep);
	free(shcmd);
	free(cmdstr);
	return 0;
}

void command(char cmd, int start, int end, int dst)
{
	switch (cmd) {
		case 'p':
		case 'n':
			printbuf(buf, cmd == 'n', start, end);
			break;
		case 'a':
			writetmp(tmp, buf);
			insertbuf(buf, end, 0, 0);
			break;
		case 'i':
			writetmp(tmp, buf);
			insertbuf(buf, start, 0, 1);
			break;
		case 'c':
			writetmp(tmp, buf);
			insertbuf(buf, start, (end - start + 1), 0);
			break;
		case 'd':
			writetmp(tmp, buf);
			dellines(buf, start, end);
			break;
		case 'm':
		case 't':
			writetmp(tmp, buf);
			movelines(buf, start, end, dst, cmd == 't');
			break;
		case 's':
			writetmp(tmp, buf);
			substitute(buf, start, end, pat, rep);
			break;
		case 'w':
			writefile(filename, buf);
			break;
		case 'u':
			undo(tmp, buf);
			break;
		case '!':
			system(shcmd);
			break;
		case 'q':
			break;
		default:
			fprintf(stderr, "?\n");
			break;
		}
}
