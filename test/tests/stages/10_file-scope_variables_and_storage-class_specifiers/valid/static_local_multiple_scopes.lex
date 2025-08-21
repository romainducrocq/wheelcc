-- Lexing ... OK
+
+
@@ Tokens @@
List[86]:
  int
  identifier(putchar)
  (
  int
  identifier(ch)
  )
  ;
  int
  identifier(print_letters)
  (
  void
  )
  {
  static
  int
  identifier(i)
  =
  const int(65)
  ;
  identifier(putchar)
  (
  identifier(i)
  )
  ;
  {
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  ;
  static
  int
  identifier(i)
  =
  const int(97)
  ;
  identifier(putchar)
  (
  identifier(i)
  )
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  ;
  }
  identifier(putchar)
  (
  const int(10)
  )
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(26)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  identifier(print_letters)
  (
  )
  ;
  }
