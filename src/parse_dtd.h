#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "xml_element.h"
#include "file_helper.h"

char *find_doctype(FILE *file, char **root_name);
XMLElement *parse_dtd(char *dtd, char *root_name);
XMLElement *parse_element(char *node_name, char **buffer, int buffer_size);
bool is_internal_doctype(char *doctype);
char *get_content_of_external_DTD(char *doctype);
char *get_DTD_filename(char *doctype);
long get_size_of_doctype(char *start);
int char_count(char *str, char character);
char *get_between_tokens(char *buffer, char *tokens);
bool is_xml_valid_char(char c);
char **split_string(char *dtd, int *size, char delim);
char *get_root_name(char *buffer);
char *get_node_name(char *buffer);
