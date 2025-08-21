-- Lexing ... OK
+
+
@@ Tokens @@
List[33]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(0)
  ;
  int
  *
  identifier(y)
  =
  &
  identifier(x)
  ;
  int
  *
  *
  identifier(z)
  =
  &
  (
  &
  identifier(x)
  )
  ;
  return
  const int(0)
  ;
  }
