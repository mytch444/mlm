atom *print_function(symbol *symbols, atom *atoms) {
	atom *ca, *a, *r, *copy, *cs;
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
				sub = malloc(sizeof(char) * (1 + copy->d->i / 10 + copy->d->i < 0 ? 1 : 0));
				sprintf(sub, "%i", copy->d->i);
			} else if (n == 'f') {
				sub = malloc(sizeof(char) * (1 + copy->d->i / 10 + copy->d->i < 0 ? 1 : 0));
				sprintf(sub, "%f", copy->d->f);
			} else if (n == 'c') {
				sub = malloc(sizeof(char) * (1 + copy->d->i / 10));
				sprintf(sub, "%c", copy->d->i);	
			} else if (n == 's') {
				for (j = 0, cs = copy->s; cs; cs = cs->next, j++);
				sub = malloc(sizeof(char) * (j + 1));
				for (j = 0, cs = copy->s; cs; cs = cs->next, j++) {
					if (!cs->d && !cs->s && !cs->f)
						break;
					if (cs->d->type != CHAR) {
						printf("BAD FORMATTING!\n");
						return NIL;
					}
					sub[j] = cs->d->i;
				}
			} else if (n == 'l') {
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

atom *read_function(symbol *symbols, atom *atoms) {
	ssize_t size;
	size_t c;
	char *buf, *b, end;
	int fd, max;
	atom *r;

	if (!atoms || !atoms->d || atoms->d->type != INT ||
			!atoms->next || !atoms->next->d || atoms->next->d->type != INT ||
			!atoms->next->next || !atoms->next->next->d || atoms->next->next->d->type != CHAR)
		return NIL;

	fd = atoms->d->i;
	max = atoms->next->d->i;
	end = atoms->next->next->d->i;
	c = sizeof(char) * max;
	buf = malloc(c);
	b = buf;

	while (1) {
		if (!read(fd, b, sizeof(char)))
			return NIL;
		if (!b || *b == end)
			break;
		b++;
	}

	r = string_to_atom_string(buf);
	return r;
}

atom *open_function(symbol *symbols, atom *atoms) {
	if (!atoms || !atoms->s || !atoms->next || !atoms->next->s)
		return NIL;

	char *string = atom_string_to_string(atoms);
	char *flag_string = atom_string_to_string(atoms->next);
	int flags;
	if (!strcmp(flag_string, "r"))
		flags = O_RDONLY;
	else if (!strcmp(flag_string, "w"))
		flags = O_WRONLY;
	else if (!strcmp(flag_string, "rw"))
		flags = O_RDWR;
	else
		return NIL;

	int o = open(string, flags);
	return data_to_atom(int_to_data(o));
}
