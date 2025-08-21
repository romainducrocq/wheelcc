-- Lexing ... OK
+
+
@@ Tokens @@
List[101]:
  extern
  char
  identifier(c)
  ;
  extern
  unsigned
  char
  identifier(uc)
  ;
  extern
  signed
  char
  identifier(sc)
  ;
  int
  identifier(update_global_chars)
  (
  void
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(c)
  !=
  const int(100)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(uc)
  !=
  const int(250)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(sc)
  !=
  const int(0)
  )
  {
  return
  const int(3)
  ;
  }
  identifier(update_global_chars)
  (
  )
  ;
  if
  (
  identifier(c)
  !=
  const int(110)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(uc)
  !=
  const int(4)
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
  -
  const int(10)
  )
  {
  return
  const int(6)
  ;
  }
  return
  const int(0)
  ;
  }
