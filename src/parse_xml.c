#include "parse_xml.h"

xml_attribute_linkedlist *create_next_element(xml_attribute_linkedlist *head)
{
  xml_attribute_linkedlist *new_attribute = malloc(sizeof(xml_attribute_linkedlist));
  new_attribute->next = NULL;
  new_attribute->value = NULL;
  head->next = new_attribute;
  return new_attribute;
}

int make_attributes(char *tag, char *subject, xml_element *element)
{
  char *start;
  size_t i = 0, j = 0, counter;
  char *tmp = (char *)calloc(strlen(tag) + 1, sizeof(char));
  strcpy(tmp, "<");
  start = strstr(subject, strcat(tmp, tag));
  tmp = start;

  xml_attribute_linkedlist *head = element->attributes;

  // Care about <b attr=">">
  while (tmp[i] != '>')
  {
    if (tmp[i] == '=')
    {
      element->number_of_attribute += 1;
      xml_attribute *attr = malloc(sizeof(xml_attribute));
      j = i;
      counter = 0;
      while (tmp[j] != ' ')
      {
        j -= 1;
        counter += 1;
      }
      attr->name = calloc(counter, sizeof(char));
      strncpy(attr->name, tmp + i - counter, counter);

      i += 2; // skip ="
      j = i;
      while (tmp[j] != '"' && tmp[j] != '\'' && j < strlen(tmp))
      {
        j += 1;
      }
      attr->value = calloc(j - i, sizeof(char));
      strncpy(attr->value, tmp + i, j - i);

      element->attributes->value = attr;
      element->attributes = create_next_element(element->attributes);
    }

    if (i > strlen(start))
    {
      fprintf(stderr, "No '>' in xml \n");
      exit(EXIT_FAILURE);
    }
    i += 1;
  }
  element->attributes = head;
  i += 1; // skip >
  return i;
}

void create_empty_xml_attribute_linkedlist(xml_element *element)
{
  element->attributes = malloc(sizeof(xml_attribute_linkedlist));
  element->attributes->value = NULL;
  element->attributes->next = NULL;
}

void print_attribute(xml_element *element)
{
  unsigned int i = 0;
  xml_attribute_linkedlist *head = element->attributes;
  printf("number of attributes: %d\n", element->number_of_attribute);
  for (i = 0; i < element->number_of_attribute; i += 1)
  {
    printf("%s=\"%s\"\n", head->value->name, head->value->value);

    head = head->next;
  }
}

void get_content(char *subject, xml_element *element)
{
  char *content = (char *)malloc(sizeof(char) * strlen(subject));
  strcpy(content, subject);
  char *end = strstr(content, "</classrooms>");
  end[0] = '\0';
  element->content = content;
}

void print_element(xml_element *element)
{
  printf("%s\n", element->name);
  print_attribute(element);
  printf("%s\n", element->content);
}

void free_element(xml_element *element)
{
  xml_attribute_linkedlist *tmp = NULL;
  unsigned int i;

  for (i = 0; i < element->number_of_attribute; i += 1)
  {
    free(element->attributes->value->name);
    free(element->attributes->value->value);
    free(element->attributes->value);
    tmp = element->attributes;
    element->attributes = element->attributes->next;
    free(tmp);
  }
  free(element->name);
  free(element->content);
  free(element);
}

xml_element *get_element(char *xml, char *tag_name)
{
  int index_of_opening_tag;
  xml_element *tag = malloc(sizeof(xml_element));
  tag->name = tag_name;
  tag->number_of_attribute = 0;
  create_empty_xml_attribute_linkedlist(tag);
  index_of_opening_tag = make_attributes(tag_name, xml, tag);
  char *start = xml + sizeof(char) * index_of_opening_tag;
  get_content(start, tag);
  print_element(tag);
  free_element(tag);
  return tag;
}