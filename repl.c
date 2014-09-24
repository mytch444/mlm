
void repl(FILE *in, int print) {
  atom *parsed, *result;

  while (!exit_repl) {
    if (print) printf("-> ");
    
    parsed = read_expression(in);
    if (!parsed)
      break;

    result = evaluate(parsed);
    if (print)
      printf("%s\n", atom_to_string(result));
  }
}
