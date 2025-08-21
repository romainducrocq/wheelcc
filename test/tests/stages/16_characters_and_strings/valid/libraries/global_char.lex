-- Lexing ... OK
+
+
@@ Tokens @@
List[45]:
  char
  identifier(c)
  =
  const int(100)
  ;
  unsigned
  char
  identifier(uc)
  =
  const int(250)
  ;
  signed
  char
  identifier(sc)
  =
  const int(0)
  ;
  int
  identifier(update_global_chars)
  (
  void
  )
  {
  identifier(c)
  =
  identifier(c)
  +
  const int(10)
  ;
  identifier(uc)
  =
  identifier(uc)
  +
  const int(10)
  ;
  identifier(sc)
  =
  identifier(sc)
  -
  const int(10)
  ;
  return
  const int(0)
  ;
  }
