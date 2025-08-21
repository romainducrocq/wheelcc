-- Lexing ... OK
+
+
@@ Tokens @@
List[92]:
  int
  identifier(i)
  =
  const int(0)
  ;
  int
  identifier(putchar)
  (
  int
  identifier(c)
  )
  ;
  int
  *
  identifier(print_A)
  (
  void
  )
  {
  identifier(putchar)
  (
  const int(65)
  )
  ;
  return
  &
  identifier(i)
  ;
  }
  int
  *
  identifier(print_B)
  (
  void
  )
  {
  identifier(putchar)
  (
  const int(66)
  )
  ;
  return
  &
  identifier(i)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  *
  identifier(print_A)
  (
  )
  +=
  const int(5)
  ;
  if
  (
  identifier(i)
  !=
  const int(5)
  )
  {
  return
  const int(1)
  ;
  }
  *
  identifier(print_B)
  (
  )
  +=
  const long(5l)
  ;
  if
  (
  identifier(i)
  !=
  const int(10)
  )
  {
  return
  const int(2)
  ;
  }
  return
  const int(0)
  ;
  }
