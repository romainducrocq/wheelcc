-- Lexing ... OK
+
+
@@ Tokens @@
List[42]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(outer)
  =
  const int(1)
  ;
  int
  identifier(foo)
  =
  const int(0)
  ;
  if
  (
  identifier(outer)
  )
  {
  extern
  int
  identifier(foo)
  ;
  extern
  int
  identifier(foo)
  ;
  return
  identifier(foo)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(foo)
  =
  const int(3)
  ;
