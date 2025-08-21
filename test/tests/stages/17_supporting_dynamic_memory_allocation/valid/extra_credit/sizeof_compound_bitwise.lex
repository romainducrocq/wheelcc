-- Lexing ... OK
+
+
@@ Tokens @@
List[143]:
  int
  identifier(main)
  (
  void
  )
  {
  static
  signed
  char
  identifier(sc)
  =
  const int(10)
  ;
  unsigned
  int
  identifier(u)
  =
  const unsigned int(10000u)
  ;
  long
  identifier(l)
  =
  -
  const int(99999)
  ;
  if
  (
  sizeof
  (
  identifier(sc)
  &=
  identifier(l)
  )
  !=
  const int(1)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  sizeof
  (
  identifier(l)
  |=
  identifier(u)
  )
  !=
  const int(8)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  sizeof
  (
  identifier(u)
  ^=
  identifier(l)
  )
  !=
  const int(4)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  sizeof
  (
  identifier(l)
  >>=
  identifier(sc)
  )
  !=
  const int(8)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  sizeof
  (
  identifier(sc)
  <<=
  identifier(sc)
  )
  !=
  const int(1)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(sc)
  !=
  const int(10)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(u)
  !=
  const unsigned int(10000u)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(l)
  !=
  -
  const int(99999)
  )
  {
  return
  const int(8)
  ;
  }
  return
  const int(0)
  ;
  }
