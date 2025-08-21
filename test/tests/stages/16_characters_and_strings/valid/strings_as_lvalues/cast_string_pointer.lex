-- Lexing ... OK
+
+
@@ Tokens @@
List[68]:
  int
  identifier(main)
  (
  void
  )
  {
  char
  *
  identifier(c)
  =
  string literal("This is a string!")
  ;
  unsigned
  char
  *
  identifier(uc)
  =
  (
  unsigned
  char
  *
  )
  identifier(c)
  ;
  if
  (
  identifier(uc)
  [
  const int(3)
  ]
  !=
  const char('s')
  )
  {
  return
  const int(1)
  ;
  }
  signed
  char
  *
  identifier(sc)
  =
  (
  signed
  char
  *
  )
  identifier(c)
  ;
  if
  (
  identifier(sc)
  [
  const int(3)
  ]
  !=
  const char('s')
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
