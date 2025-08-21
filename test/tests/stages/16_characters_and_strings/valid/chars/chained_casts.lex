-- Lexing ... OK
+
+
@@ Tokens @@
List[63]:
  unsigned
  int
  identifier(ui)
  =
  const unsigned int(4294967200u)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  identifier(ui)
  =
  (
  unsigned
  int
  )
  (
  unsigned
  char
  )
  identifier(ui)
  ;
  if
  (
  identifier(ui)
  !=
  const int(160)
  )
  {
  return
  const int(1)
  ;
  }
  int
  identifier(i)
  =
  (
  int
  )
  (
  signed
  char
  )
  identifier(ui)
  ;
  if
  (
  identifier(i)
  !=
  -
  const int(96)
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
