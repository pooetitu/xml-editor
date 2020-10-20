#include "parse_dtd.h"

// TODO attention http
char *find_doctype(FILE *file)
{

  char *buffer = file_get_content(file);
  char *start = strstr(buffer, "<!DOCTYPE");
  char size_of_doctype = get_size_of_doctype(start);
  char *doctype = (char *)malloc(sizeof(char) * size_of_doctype);
  if (!doctype)
  {
    fprintf(stderr, "Failed to allocate memory for {doctype} [find_doctype]");
    exit(EXIT_FAILURE);
  }
  strncpy(doctype, start, size_of_doctype);
  free(buffer);
  if (is_internal_doctype(doctype))
  {
    return doctype;
  }
  else
  {
    return get_content_of_external_DTD(doctype);
  }
}

char *get_content_of_external_DTD(char *doctype)
{
  char *res = NULL;
  char *external_DTD_filename = NULL;
  FILE *external_DTD_file = NULL;

  external_DTD_filename = get_DTD_filename(doctype);
  external_DTD_file = fopen(external_DTD_filename, "r");

  if (external_DTD_file == NULL)
  {
    fprintf(stderr, "Failed to open file: %s\n", external_DTD_filename);
    fclose(external_DTD_file);
    exit(EXIT_FAILURE);
  }

  res = file_get_content(external_DTD_file);
  free(external_DTD_filename);
  free(doctype);
  fclose(external_DTD_file);
  return res;
}

char *get_DTD_filename(char *doctype)
{
  char *res;
  long size_of_doctype = strlen(doctype);
  int n = 0;
  int start = 0;
  int end = 0;
  while (n < size_of_doctype)
  {
    if (doctype[n] == '"')
    {
      if (start == 0)
      {
        start = n + 1;
      }
      else
      {
        end = n;
        break;
      }
    }
    n += 1;
  }
  res = (char *)malloc(sizeof(char) * (end - start));
  if (res == NULL)
  {
    fprintf(stderr, "Memory allocation failed [get_DTD_filename]");
    exit(EXIT_FAILURE);
  }
  strncpy(res, doctype + start, end - start);
  return res;
}

long get_size_of_doctype(char *start)
{
  long size_of_buffer = strlen(start);
  long n = 1;
  int nb_open_tags = 0;
  int exit_loop = 0;

  while (n < size_of_buffer && exit_loop == 0)
  {
    if (start[n] == '<')
    {
      nb_open_tags += 1;
    }
    else if (start[n] == '>')
    {
      if (nb_open_tags == 0)
      {
        exit_loop = 1;
      }
      else
      {
        nb_open_tags -= 1;
      }
    }
    n += 1;
  }
  return n;
}

bool is_internal_doctype(char *doctype)
{
  if (strstr(doctype, "SYSTEM") == NULL)
  {
    return true;
  }
  return false;
}

int char_count(char *str, char character)
{
  int counter = 0;
  for (size_t i = 0; i < strlen(str); i++)
  {
    if (str[i] == character)
    {
      counter++;
    }
  }
  return counter;
}

XMLElement *parse_dtd(char *dtd)
{
  printf("Starting to parse dtd\n");
  int buff_size = char_count(dtd, '>');
  char **buff = malloc(sizeof(char *) * buff_size);
  if (buff == NULL)
  {
    fprintf(stderr, "Failed to allocate memory [parse_dtd]\n");
    exit(EXIT_FAILURE);
  }
  char tmp[strlen(dtd)];
  strcpy(tmp, dtd);
  XMLElement *parent = NULL;
  int i = 0;
  buff[i] = strtok(tmp, ">");
  while (buff[i] != NULL)
  {
    buff[++i] = strtok(NULL, ">");
  }
  for (int i = 0; i < buff_size; i++)
  {
    printf("%s\n", buff[i]);
  }
  free(buff);
  return parent;
}
