-- Lexing ... OK
+
+
@@ Tokens @@
List[38]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(1)
  ;
  int
  identifier(y)
  =
  const int(2)
  ;
  int
  identifier(z)
  =
  const int(3)
  ;
  int
  *
  identifier(ptr)
  =
  &
  (
  identifier(x)
  ?
  identifier(y)
  :
  identifier(z)
  )
  ;
  return
  const int(0)
  ;
  }
