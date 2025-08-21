-- Lexing ... OK
+
+
@@ Tokens @@
List[39]:
  struct
  identifier(inner)
  {
  int
  identifier(i)
  ;
  }
  ;
  struct
  identifier(outer)
  {
  struct
  identifier(inner)
  identifier(foo)
  ;
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(outer)
  identifier(x)
  =
  {
  {
  const int(1)
  ,
  const int(2)
  }
  }
  ;
  return
  const int(0)
  ;
  }
