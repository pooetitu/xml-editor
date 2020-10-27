#include "parse_dtd.h"

typedef struct xml_attribute
{
  char *name;
  char *value;

} xml_attribute;
typedef struct xml_element
{
  char *name;
  xml_attribute *attributes;
  char *content;
} xml_element;

xml_element *get_root_balise(char *xml, char *root_name);

/**
 * I use ARGV to give relative path to xml
 */
int main(int argc, char **argv)
{
  if (argc < 2)
  {
    fprintf(stderr, "Error attending xml file in parameters\n");
    return EXIT_FAILURE;
  }
  FILE *file = fopen(argv[1], "r");
  if (file == NULL)
  {
    fclose(file);
    fprintf(stderr, "Error opening file %s\n", argv[1]);
    return EXIT_FAILURE;
  }
  char *root_name;
  char *dtd = find_doctype(file, &root_name);
  fclose(file);

  char *xml = "<classrooms attr='fefe' b='ef' c='' derrerzer='qs'>"
              "< classroom > AL</ classroom>"
              "<classroom> IABD</ classroom>"
              "<classroom> MOC</ classroom>"
              "<classroom> IBC</ classroom>"
              "</ classrooms> ";

  get_root_balise(xml, root_name);

  parse_dtd(dtd, root_name);
  free(dtd);
  free(root_name);
  return EXIT_SUCCESS;
}

bool isWhiteSpaceCharacter(char c)
{
  unsigned int size = 8;
  unsigned int i;
  char whiteSpaceCharacter[] = {32, 9, 10, 11, 12, 13, 133, 160};
  for (i = 0; i < size; i += 1)
  {
    if (whiteSpaceCharacter[i] == c)
    {
      return true;
    }
  }
  return false;
}

xml_element *get_root_balise(char *xml, char *root_name)
{
  xml_element *res = malloc(sizeof(xml_element));
  res->name = root_name;
  // char *start_attr;
  // char *end_attr;
  char *start;
  char *tag;
  char *tmp = (char *)calloc(strlen(root_name) + 1, sizeof(char));
  strcpy(tmp, "<");
  start = strstr(xml, strcat(tmp, root_name));
  tmp = start;
  size_t i = 0;
  size_t j = 0;
  size_t counter;
  // Care about <b attr=">">
  while (tmp[i] != '>')
  {
    if (tmp[i] == '=')
    {
      xml_attribute *new_attrbute = malloc(sizeof(xml_attribute));
      j = i;
      counter = 0;
      while (tmp[j] != ' ')
      {
        j -= 1;
        counter += 1;
      }
      new_attrbute->name = calloc(counter, sizeof(char));
      strncpy(new_attrbute->name, tmp + i - counter, counter);
      printf("attr_name: %s\n", new_attrbute->name);
      i += 2; // skip first "
      j = i;
      while (tmp[j] != '"' && tmp[j] != '\'' && j < strlen(tmp))
      {
        j += 1;
      }
      new_attrbute->value = calloc(j - i, sizeof(char));
      strncpy(new_attrbute->value, tmp + i, j - i);
      printf("attr_value: %s\n", new_attrbute->value);
    }

    if (i > strlen(start))
    {
      fprintf(stderr, "No '>' in xml \n");
      exit(EXIT_FAILURE);
    }
    i += 1;
  }
  tag = malloc(sizeof(char) * (i + 2));
  strncpy(tag, start, (i + 1) * sizeof(char));
  printf("%s\n", tag);
  return res;
  // GET XML STRING
  // <root_name ... >
  // stack qui ouvre et qui ferme
  // <root_name ... >
  // ELEMENT name;
}