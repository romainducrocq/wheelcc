-- Lexing ... OK
+
+
@@ Tokens @@
List[103]:
  int
  identifier(main)
  (
  void
  )
  {
  unsigned
  char
  identifier(uc)
  =
  const int(255)
  ;
  if
  (
  (
  identifier(uc)
  >>
  const int(3)
  )
  !=
  const int(31)
  )
  {
  return
  const int(2)
  ;
  }
  signed
  char
  identifier(sc)
  =
  -
  const int(127)
  ;
  char
  identifier(c)
  =
  const int(5)
  ;
  if
  (
  (
  identifier(sc)
  >>
  identifier(c)
  )
  !=
  -
  const int(4)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  (
  (
  -
  (
  identifier(c)
  <<
  const unsigned long(3ul)
  )
  )
  >>
  const int(3)
  )
  !=
  -
  const int(5)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  (
  -
  (
  identifier(uc)
  <<
  const unsigned int(5u)
  )
  >>
  const unsigned int(5u)
  )
  !=
  -
  const long(255l)
  )
  {
  return
  const int(5)
  ;
  }
  return
  const int(0)
  ;
  }
