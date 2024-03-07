#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "bled.h"

int main(int argc, char **argv)
{
	if (argc < 2)
		return 1;
	FILE *tmp = tmpfile();

	char *filename = argv[1];
	char cmd[100] = "";

	char **buf = readfile(filename);

	int start = 1;
	int end = END;
	while (strcmp(cmd, "q\n")) {
		fgets(cmd, 100, stdin);

		int i = 0;

		while (!isalpha(cmd[i])) {
			if (isdigit(cmd[i])) {
				for (end = 0; isdigit(cmd[i]); i++)
					end = 10 * end + (cmd[i] - '0');

				start = end;
				if (cmd[i] == ',')
					i++;
				else
					break;
				if (cmd[i] == '$') {
					end = END;
					i++;
				} else {
					for (end = 0; isdigit(cmd[i]); i++)
						end = 10 * end + (cmd[i] - '0');
				}
			} else if (cmd[i] == ',') {
				start = 1;
				end = END;
				i++;
			}
		}

		switch (cmd[i]) {
		case 'p':
		case 'n':
			printbuf(buf, cmd[i] == 'n', start, end);
			break;
		case 'a':
		case 'i':
		case 'c':
			writetmp(tmp, buf);
			insertbuf(buf, end, cmd[i] == 'c', cmd[i] == 'i');
			break;
		case 'd':
			writetmp(tmp, buf);
			dellines(buf, start, end);
			break;
		case 'w':
			writefile(filename, buf);
			break;
		case 'u':
			undo(tmp, buf);
			break;
		case 'q':
			break;
		default:
			fprintf(stderr, "?\n");
			break;
		}
	}
	return 0;
}
