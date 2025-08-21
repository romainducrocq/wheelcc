-- Lexing ... OK
+
+
@@ Tokens @@
List[44]:
  int
  identifier(read_x)
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
  extern
  int
  identifier(x)
  ;
  if
  (
  identifier(x)
  !=
  const int(0)
  )
  return
  const int(1)
  ;
  identifier(x)
  =
  const int(3)
  ;
  if
  (
  identifier(read_x)
  (
  )
  !=
  const int(3)
  )
  return
  const int(1)
  ;
  return
  const int(0)
  ;
  }
