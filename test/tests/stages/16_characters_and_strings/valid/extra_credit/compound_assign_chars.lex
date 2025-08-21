-- Lexing ... OK
+
+
@@ Tokens @@
List[121]:
  int
  identifier(main)
  (
  void
  )
  {
  static
  char
  identifier(c)
  =
  const int(100)
  ;
  char
  identifier(c2)
  =
  const int(100)
  ;
  identifier(c)
  +=
  identifier(c2)
  ;
  if
  (
  identifier(c)
  !=
  -
  const int(56)
  )
  {
  return
  const int(1)
  ;
  }
  static
  unsigned
  char
  identifier(uc)
  =
  const int(200)
  ;
  identifier(c2)
  =
  -
  const int(100)
  ;
  identifier(uc)
  /=
  identifier(c2)
  ;
  if
  (
  identifier(uc)
  !=
  const int(254)
  )
  {
  return
  const int(2)
  ;
  }
  identifier(uc)
  -=
  const double(250.0)
  ;
  if
  (
  identifier(uc)
  !=
  const int(4)
  )
  {
  return
  const int(3)
  ;
  }
  static
  signed
  char
  identifier(sc)
  =
  const int(70)
  ;
  identifier(sc)
  =
  -
  identifier(sc)
  ;
  identifier(sc)
  *=
  identifier(c)
  ;
  if
  (
  identifier(sc)
  !=
  const int(80)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  (
  identifier(sc)
  %=
  identifier(c)
  )
  !=
  const int(24)
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
