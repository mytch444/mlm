int closing_bracket_pos(char *string, int open) {
  int i;
  int brackets = 0;
  for (i = open; string[i] != '\0'; i++) {
    if (string[i] == '(') brackets++;
    if (string[i] == ')') {
      if (brackets == 1) return i;
      else brackets--;
    }
  }
  
  return -1;
}

char *string_cut(char *string, int start, int stop) {
  int i;
  char *new = malloc(sizeof(char) * (stop - start + 1));
  for (i = 0; i < stop - start; i++) new[i] = string[start + i];
  new[i] = '\0';
  return new;
}

int string_is_int(char *string) {
  int i = 0;
  if (string[i] == '-' && string[i + 1])
    i++;
  for (; string[i]; i++)
    if (!(string[i] >= '0' && string[i] <= '9'))
      return 0;
  return 1;
}

int string_is_float(char *string) {
  int i = 0;
  int point = 0;
  if (string[i] == '-' && string[i + 1])
    i++;
  for (; string[i]; i++) {
    if (string[i] == '.' && !point) {
      point = 1;
      continue;
    }
    if (!(string[i] >= '0' && string[i] <= '9'))
      return 0;
  }
  return 1;
}

int string_is_char(char *string) {
  int i;
  if (string[0] == '\'' && string[2] == '\'')
    return 1;
  return 0;
}

int string_is_string(char *string) {
  int i;
  for (i = 0; string[i] && string[i + 1]; i++);
  if (string[0] == '\"' && string[i] == '\"')
    return 1;
  return 0;
}

symbol *find_symbol(char *name) {
  symbol *s;
  for (s = symbols; s; s = s->next) {
    if (strcmp(s->name, name) == 0) {
      return s;
    }
  }

  return NULL;
}

atom *swap_symbols(atom *atoms) {
  atom *a, *s;
  for (a = atoms; a; a = a->next) {
    if (a->sym && a->sym->atoms) {
      s = a->sym->atoms;
      a->d = copy_data(s->d);
      a->s = copy_atom(s->s);
      a->f = copy_function(s->f);
      a->sym = NULL;
    } else if (a->s) {
      a->s = swap_symbols(a->s);
    }
  }

  return atoms;
}

atom *data_to_atom(data *d) {
  atom *a = malloc(sizeof(atom));
  a->d = d;
  a->s = NULL;
  a->f = NULL;
  a->sym = NULL;
  a->next = NULL;
  return a;
}

data *char_to_data(char c) {
  data *d = malloc(sizeof(data));
  
  d->type = CHAR;
  d->i = c;
  d->f = 0;
  
  return d;
}

data *int_to_data(int i) {
  data *d = malloc(sizeof(data));
  
  d->type = INT;
  d->i = i;
  d->f = 0;
  
  return d;
}

data *float_to_data(double f) {
  data *d = malloc(sizeof(data));
  
  d->type = FLOAT;
  d->f = f;
  d->i = 0;
  
  return d;
}

data *string_to_data(char *string) {
  if (string_is_int(string)) {
    return int_to_data(atoi(string));
    
  } else if (string_is_float(string)) {
    return float_to_data(atof(string));
    
  } else if (string_is_char(string)) {
    return char_to_data(string[1]);
    
  } else return NULL;
}

atom *string_to_atom_string(char *string) {
  atom *a, *t;
  int j;
  
  a = malloc(sizeof(atom));
  a->d = NULL;
  a->s = malloc(sizeof(atom));
  a->f = NULL;
  a->sym = NULL;
  a->next = NULL;

  t = a->s;
  for (j = 0; string[j]; j++) {
    t->d = char_to_data(string[j]);
    t->s = NULL;
    t->f = NULL;
    t->sym = NULL;
    t->next = malloc(sizeof(atom));
    t = t->next;
    t->d = NULL;
    t->f = NULL;
    t->s = NULL;
    t->sym = NULL;
    t->next = NULL;
  }

  return a;
}

atom *constant_to_atom(char *name) {
  atom *a; 

  a = malloc(sizeof(atom));
  a->d = NULL;
  a->s = NULL;
  a->f = NULL;
  a->sym = NULL;
  a->next = NULL;

  data *d = string_to_data(name);
  if (d) {
    a->d = d;
  } else {
    symbol *s = find_symbol(name);
    if (s) {
      //      printf("found\n");
      a->sym = s;
    } else {
      //      printf("making tmp\n");
      a->sym = malloc(sizeof(symbol));
      a->sym->name = name;
      a->sym->atoms = NULL;
      a->sym->next = NULL;
    }
  }

  return a;
}

atom *parse(char *string) {
  int num, i, start, end, j;
  atom *handle, *atoms, *t;
  
  handle = malloc(sizeof(atom));
  handle->next = NULL;
  atoms = handle;
  num = 0;
  i = 0;
  while (string[i]) {
    for (; string[i] && isspace(string[i]); i++);
    
    if (!string[i])
      break;
    
    if (string[i] == '\"') {
      start = i + 1;
      for (i++; string[i] && string[i] != '\"'; i++);
      i++;
      if (start >= i)
	return NIL;

      atoms->next = string_to_atom_string(string_cut(string, start, i - 1));
      atoms = atoms->next;
      num++;
    } else if (ischar(string[i])) {
      start = i;
      for (; string[i] && ischar(string[i]); i++);
      if (start >= i)
	return NIL;
      
      char *name = string_cut(string, start, i);

      atoms->next = constant_to_atom(name);
      atoms = atoms->next;
      num++;
    } else if (string[i] == '(') {
      start = i + 1;
      end = closing_bracket_pos(string, start - 1);
      if (start > end) {
	printf("Error parsing\n");
	return NULL;
      }
      
      atom *sub = parse(string_cut(string, start, end));
      
      if (sub) {
	atoms->next = malloc(sizeof(atom));
	atoms = atoms->next;
	atoms->next = NULL;
	atoms->d = NULL;
	atoms->f = NULL;
	atoms->sym = NULL;
	atoms->s = sub;
	
	atom *a;
	for (a = atoms->s; a && a->next; a = a->next);
	a->next = NIL;
      } else {
	atoms->next = NIL;
	atoms = atoms->next;
      }
      
      num++;
      i = end + 1;
    } else {
      printf("I have no idea what to do with this char '%c'\n", string[i]);
      return NULL;
    }
  }
  
  return handle->next;
}

atom *read_expression(FILE *in) {
  char line[500];
  char string[5000];
  atom *parsed;
  int c, d, open, close, inquote, inchar;
    
  d = c = open = close = inquote = inchar = 0;
  string[0] = '\0';
  line[0] = '\0';
    
  do {
    while (!line[c] || isspace(line[c])) {
      if (!fgets(line, sizeof(char) * 500, in))
	return NULL;

      for (c = 0; line[c] && isspace(line[c]); c++);
      if (!line[c]) {
	continue;
      }
    }
    
    for (c = 0; line[c]; c++) {
      if (!inquote && line[c] == '\'') inchar = !inchar;
      else if(line[c] == '\"') inquote = !inquote;
      else if (line[c] == '(' && !inquote && !inchar) open++;
      else if (line[c] == ')' && !inquote && !inchar) close++;
      else if (iscomment(line[c]) && !inquote && !inchar) break;

      string[d + c] = line[c];
    }

    d += c;
    string[d] = '\0';
    
  } while (open != close || inquote || inchar);
  
  for (c = 0; string[c] && string[c + 1]; c++);
  if (c == 0)
    return read_expression(in);
  else if (string[c] == '\n')
    string[c] = '\0';

  parsed = parse(string);
  return parsed;
}
