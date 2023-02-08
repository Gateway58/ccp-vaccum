#include "fixup.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
uint8_t is_num(char c){
	return c >= 0x30 && c <= 0x39;
}
uint8_t get_line(char **dest, uint64_t *char_len, FILE *file){
	if (*char_len == 0){
		*char_len = 1;
		*dest = malloc(1);
	}
	char c;
	uint64_t i = 0;
	while( (c = fgetc(file)) != EOF && c != 0xA){
		(*dest)[i++] = c;
		if (i >= *char_len){
			*char_len *= 2;
			*dest = realloc(*dest, *char_len * sizeof(char));
		}
	}
	(*dest)[i] = 0;
	return i != 0;
}
void process(char *path, char *content, uint64_t content_len, char *ext, uint64_t ext_len){
	FILE *file = fopen(path, "r");
	if (!file){
		perror("Couldn't find file");
		return;
	}
	char *line;
	uint64_t line_n = 0;
	char *filename = malloc((3 + ext_len) * sizeof(char));
	uint64_t end_num = 0;
	while (get_line(&line, &line_n, file)){
		if (is_num(*line)){
			for(end_num = 0; is_num(line[end_num]); end_num += 1);
			if (end_num > 2 ){
				perror("to long");
			}
			uint64_t i;
			for(i = end_num; line[i]; i += 1){
				strncpy(filename, line, end_num);
				*(filename + end_num) = line[i];
				strncpy(filename + end_num + 1, ext, ext_len);
				FILE *creat = fopen(filename, "w");
				fwrite(content, sizeof(char), content_len - 1, creat);
				fclose(creat);
			}

		}
		else{
			*filename = *line;
			for(end_num = 1; is_num(line[end_num]); end_num += 1);
			strncpy(filename + 1, line + 1, end_num);
			strncpy(filename + end_num, ext, ext_len);
			printf("%s\n", filename);
			FILE *creat = fopen(filename, "w");
			fwrite(content, sizeof(char), content_len - 1, creat);
			fclose(creat);
		}
	}
	fclose(file);
	free(line);
	free(filename);
}