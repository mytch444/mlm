atom *print_function(atom *atoms) {
  atom *ca, *a, *r, *copy;
  char string[1000], c, n, *sub;
  int i, j;

  if (!atoms->s)
    return NIL;

  a = atoms->next;

  string[0] = '\0';  
  for (i = 0, ca = atoms->s; ca; ca = ca->next) {
    if (!ca->d || ca->d->type != CHAR)
      break;

    c = ca->d->i;

    if (c == '%') {
      if (!ca->next || !ca->next->d || ca->next->d->type != CHAR) {
	printf("Bad formatting at %i. You must continue with string.", i);
	return NIL;
      }

      ca = ca->next;
      n = ca->d->i;

      if (n == '%') {
	string[i++] = '%';
	string[i] = '\0';
	continue;
      }
	
      if (!a)
	continue;

      copy = copy_atom(a);
      copy->next = NULL;

      if (n == 'i') {
	sub = malloc(sizeof(char) * (1 + copy->d->i / 10));
	sprintf(sub, "%i", copy->d->i);
      } else if (n == 'f') {
	sub = malloc(sizeof(char) * (1 + copy->d->i / 10));
	sprintf(sub, "%f", copy->d->f);
      } else if (n == 'c') {
	sub = malloc(sizeof(char) * (1 + copy->d->i / 10));
	sprintf(sub, "%c", copy->d->i);	
      } else if (n == 's' || n == 'l') {
	sub = atom_to_string(copy);
      } else {
	printf("Bad formatting at %i, %c not recognised!\n", i, n);
      }
	
      a = a->next;

      for (j = 0; sub[j]; j++)
	string[i + j] = sub[j];
      string[i + j] = '\0';
      
      i += j;
      
    } else {
      string[i++] = c;
      string[i] = '\0';
    }
  }

  printf("%s\n", string);
  return NIL;
}

atom *read_function(atom *atoms) {
  char line[1000];
  int i;
  fgets(line, sizeof(char) * 1000, stdin);
  for (i = 0; line[i] && line[i] != '\n'; i++);
  line[i] = '\0';

  return string_to_atom_string(line);
}
