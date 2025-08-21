-- Lexing ... OK
+
+
@@ Tokens @@
List[39]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(a)
  =
  const int(0)
  ;
  {
  int
  identifier(b)
  =
  const int(4)
  ;
  identifier(a)
  =
  identifier(b)
  ;
  }
  {
  int
  identifier(b)
  =
  const int(2)
  ;
  identifier(a)
  =
  identifier(a)
  -
  identifier(b)
  ;
  }
  return
  identifier(a)
  ;
  }
