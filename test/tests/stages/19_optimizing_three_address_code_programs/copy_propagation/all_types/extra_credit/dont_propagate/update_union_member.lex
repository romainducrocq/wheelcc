-- Lexing ... OK
+
+
@@ Tokens @@
List[50]:
  union
  identifier(u)
  {
  long
  identifier(l)
  ;
  int
  identifier(i)
  ;
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  static
  union
  identifier(u)
  identifier(u1)
  =
  {
  const int(20)
  }
  ;
  union
  identifier(u)
  identifier(u2)
  =
  {
  const int(3)
  }
  ;
  identifier(u1)
  =
  identifier(u2)
  ;
  identifier(u2)
  .
  identifier(i)
  =
  const int(0)
  ;
  return
  identifier(u1)
  .
  identifier(i)
  ;
  }
