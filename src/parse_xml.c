#include "parse_xml.h"

xml_attribute_linkedlist *create_next_element(XML_element *element)
{
  xml_attribute_linkedlist *new_attribute = malloc(sizeof(xml_attribute_linkedlist));
  new_attribute->next = element->attributes;
  element->attributes = new_attribute;
  element->number_of_attribute += 1;
  new_attribute->value = NULL;
  return new_attribute;
}

void get_attr_name(int index, char *subject, xml_attribute *attr)
{
  int j, counter;
  j = index;
  counter = 0;
  while (subject[j] != ' ')
  {
    counter += 1;
    j -= 1;
  }
  attr->name = calloc(counter + 1, sizeof(char));
  strncpy(attr->name, subject + index - counter + 1, counter - 1);
  attr->name[counter] = '\0';
}

void get_attr_value(int index, char *subject, xml_attribute *attr)
{
  size_t j;
  j = index;
  while (subject[j] != '"' && subject[j] != '\'' && j < strlen(subject))
  {
    j += 1;
  }
  attr->value = calloc(j - index, sizeof(char));
  strncpy(attr->value, subject + index, j - index);
  attr->name[j] = '\0';
}

int make_attributes(char *tag, char *subject, XML_element *element)
{
  char *start;
  size_t i = 0, j = 0, counter;
  char *tmp = (char *)calloc(strlen(tag) + 1, sizeof(char));
  strcpy(tmp, "<");
  start = strstr(subject, strcat(tmp, tag));
  free(tmp);

  while (start[i] != '>')
  {
    if (start[i] == '=')
    {
      xml_attribute *attr = malloc(sizeof(xml_attribute));

      get_attr_name(i, start, attr);

      i += 2; // skip ="

      get_attr_value(i, start, attr);

      element->attributes = create_next_element(element);
      element->attributes->value = attr;
    }

    if (i > strlen(start))
    {
      fprintf(stderr, "No '>' in xml \n");
      exit(EXIT_FAILURE);
    }
    i += 1;
  }
  if (start[i - 1] == '/')
  {
    element->autoclosing = true;
  }
  else
  {
    element->autoclosing = false;
  }
  i += 1; // skip >
  return i;
}

void create_empty_xml_attribute_linkedlist(XML_element *element)
{
  element->attributes = malloc(sizeof(xml_attribute_linkedlist));
  element->attributes->value = NULL;
  element->attributes->next = NULL;
}

xml_attribute **attributes_to_array(XML_element *element)
{
  xml_attribute **res = malloc(sizeof(xml_attribute_linkedlist *) * element->number_of_attribute);
  xml_attribute_linkedlist *head = element->attributes;

  for (unsigned int i = 0; i < element->number_of_attribute; i += 1)
  {
    res[i] = head->value;
    head = head->next;
  }
  return res;
}

void print_attribute(XML_element *element)
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

void get_content(char *subject, XML_element *element)
{
  char *content = (char *)malloc(sizeof(char) * strlen(subject));
  strcpy(content, subject);
  // TODO Autoclosing tag
  char *closing_tag = malloc((strlen(element->name) + 3) * sizeof(char));
  strcpy(closing_tag, "</");
  strcat(closing_tag, element->name);
  strcat(closing_tag, ">");
  char *end = strstr(content, closing_tag);
  if (end == NULL)
  {
    fprintf(stderr, "%s n'a pas de balise fermante\n", element->name);
    exit(EXIT_FAILURE);
  }
  end[0] = '\0';
  element->content = content;
  free(closing_tag);
}

void print_element(XML_element *element)
{
  printf("%s\n", element->name);
  print_attribute(element);
  printf("number of childs: %d\n", element->childs_count);
  printf("capacity of childs: %d\n", element->childs_capacity);
  printf("deepness: %d\n", element->deepness);

  if (element->parent != NULL)
  {
    printf("parent: %s\n", element->parent->name);
  }
  else
  {
    printf("parent: NULL\n");
  }

  for (int i = 0; i < element->childs_count; i += 1)
  {
    print_element(element->childs[i]);
  }
  if (element->autoclosing == false)
  {
    printf("%s\n", element->content);
  }
  printf("\n");
}

void free_element(XML_element *element)
{
  xml_attribute_linkedlist *tmp = NULL;
  unsigned int i;
  for (int j = 0; j < element->childs_count; j += 1)
  {
    free_element(element->childs[j]);
  }
  free(element->childs);
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
  if (!element->autoclosing)
  {
    free(element->content);
  }
  free(element);
}

char *find_start(char *xml, XML_element *element)
{
  int length_of_opening_tag = strlen(element->name) + (2 * sizeof(char));
  char tmp[length_of_opening_tag];
  strcpy(tmp, "<");
  strcat(tmp, element->name);
  char *str = strstr(xml, tmp);
  if (str == NULL)
  {
    fprintf(stderr, "Error root Name is not equal to %s\n", element->name);
    exit(EXIT_FAILURE);
  }
  while (str[strlen(element->name) + 1] != '>' && str[strlen(element->name) + 1] != ' ')
  {
    str = strstr(str + sizeof(char), tmp);
  }
  return str;
}

XML_element *get_element(char *xml, char *tag_name)
{
  int index_of_opening_tag;
  XML_element *element = malloc(sizeof(XML_element));
  element->parent = NULL;
  element->name = malloc(sizeof(char) * strlen(tag_name));
  strcpy(element->name, tag_name);
  element->number_of_attribute = 0;
  xml = find_start(xml, element);
  index_of_opening_tag = make_attributes(tag_name, xml, element);
  char *start = xml + sizeof(char) * index_of_opening_tag;
  if (element->autoclosing == false)
  {
    get_content(start, element);
  }
  // print_element(element);
  // free_element(element);
  return element;
}

void realloc_childs(XML_element *element)
{
  XML_element **new_array = malloc(sizeof(XML_element *) * (element->childs_capacity * 2));
  element->childs_capacity *= 2;
  for (int i = 0; i < element->childs_count; i += 1)
  {
    new_array[i] = element->childs[i];
  }
  free(element->childs);
  element->childs = new_array;
}

XML_element *get_next_element(char *xml, XML_element *parent, int deepness)
{
  int i = 1;
  int start, end;
  char *name;
  long max_size = strlen(xml);
  while (!isspace(xml[i]) && xml[i] != '>' && xml[i] != '/' && i < max_size)
  {
    i += 1;
  }
  start = 1;
  end = i - start;

  name = malloc(sizeof(char) * end);
  strncpy(name, xml + start, end);
  name[end] = '\0';

  XML_element *element = get_element(xml, name);
  element->childs = malloc(sizeof(XML_element *) * 5);
  element->childs_count = 0;
  element->childs_capacity = 5;
  element->parent = parent;
  element->deepness = deepness;

  if (parent != NULL)
  {
    if (parent->childs_count == parent->childs_capacity)
    {
      realloc_childs(parent);
    }
    parent->childs[parent->childs_count] = element;
    parent->childs_count += 1;
  }
  free(name);
  return element;
}

bool check_is_comment(char *s)
{
  if (s[1] == '!')
  {

    return true;
  }
  return false;
}

bool check_is_version(char *s)
{
  if (s[1] == '?')
  {
    return true;
  }
  return false;
}

bool check_is_doctype(char *s)
{
  if (strncmp(s, "<!DOCTYPE ", 10) == 0)
  {
    return true;
  }
  return false;
}

bool check_is_balise(char **xml)
{
  int size_of_dtd;

  if (check_is_doctype(*xml) == true)
  {
    size_of_dtd = get_size_of_doctype(*xml);
    *xml = *xml + sizeof(char) * size_of_dtd;
    return false;
  }
  if (check_is_comment(*xml) == true)
  {
    *xml = strstr(*xml, "-->");
    return false;
  }
  if (check_is_version(*xml) == true)
  {
    *xml = strstr(*xml, "?>");
    return false;
  }
  return true;
}

bool is_closing_tag(char *s)
{
  if (s[1] == '/')
  {
    return true;
  }
  return false;
}

XML_element *parse_xml(char *xml)
{
  XML_element *root;
  XML_element *current_element = NULL;
  int deepness = 0;

  while ((xml = strchr(xml, '<')) != NULL)
  {
    if (!check_is_balise(&xml))
    {
      continue;
    }

    if (is_closing_tag(xml))
    {
      if (current_element == NULL)
      {
        fprintf(stderr, "Error closing tag when no one was open.\n");
        exit(EXIT_FAILURE);
      }
      deepness -= 1;
      current_element = current_element->parent;
    }
    else
    {
      current_element = get_next_element(xml, current_element, deepness);
      if (current_element->parent == NULL)
      {
        root = current_element;
      }
      if (current_element->autoclosing == false)
      {
        deepness += 1;
      }
      else
      {
        current_element = current_element->parent;
      }
    }
    xml = xml + sizeof(char) * 1;
  }
  return root;
}
