-- Lexing ... OK
+
+
@@ Tokens @@
List[59]:
  int
  identifier(putchar)
  (
  int
  identifier(ch)
  )
  ;
  int
  identifier(print_alphabet)
  (
  void
  )
  {
  static
  int
  identifier(count)
  =
  const int(0)
  ;
  identifier(putchar)
  (
  identifier(count)
  +
  const int(65)
  )
  ;
  identifier(count)
  =
  identifier(count)
  +
  const int(1)
  ;
  if
  (
  identifier(count)
  <
  const int(26)
  )
  {
  identifier(print_alphabet)
  (
  )
  ;
  }
  return
  identifier(count)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  identifier(print_alphabet)
  (
  )
  ;
  }
