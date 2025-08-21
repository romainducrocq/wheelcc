-- Lexing ... OK
+
+
@@ Tokens @@
List[72]:
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
  const int(135)
  ;
  char
  identifier(c)
  =
  -
  const int(116)
  ;
  if
  (
  (
  identifier(uc)
  &
  identifier(c)
  )
  !=
  const int(132)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  (
  identifier(uc)
  |
  identifier(c)
  )
  !=
  -
  const int(113)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  (
  (
  identifier(c)
  ^
  const unsigned int(1001u)
  )
  |
  const long(360l)
  )
  !=
  const int(4294966637)
  )
  {
  return
  const int(3)
  ;
  }
  return
  const int(0)
  ;
  }
