-- Lexing ... OK
+
+
@@ Tokens @@
List[37]:
  int
  identifier(main)
  (
  void
  )
  {
  static
  int
  identifier(i)
  =
  const int(2)
  ;
  static
  int
  identifier(j)
  =
  const int(3)
  ;
  int
  identifier(cmp)
  =
  identifier(i)
  <
  identifier(j)
  ;
  if
  (
  !
  identifier(cmp)
  )
  return
  const int(1)
  ;
  return
  const int(0)
  ;
  }
