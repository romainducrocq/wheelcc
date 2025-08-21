-- Lexing ... OK
+
+
@@ Tokens @@
List[46]:
  union
  identifier(u1)
  {
  int
  identifier(a)
  ;
  }
  ;
  union
  identifier(u2)
  {
  int
  identifier(a)
  ;
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  union
  identifier(u1)
  identifier(x)
  =
  {
  const int(10)
  }
  ;
  union
  identifier(u2)
  identifier(y)
  =
  {
  const int(11)
  }
  ;
  identifier(x)
  =
  identifier(y)
  ;
  return
  const int(0)
  ;
  }
