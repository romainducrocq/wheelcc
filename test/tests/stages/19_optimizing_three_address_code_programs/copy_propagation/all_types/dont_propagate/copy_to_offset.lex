-- Lexing ... OK
+
+
@@ Tokens @@
List[54]:
  struct
  identifier(s)
  {
  int
  identifier(x)
  ;
  int
  identifier(y)
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
  struct
  identifier(s)
  identifier(s1)
  =
  {
  const int(1)
  ,
  const int(2)
  }
  ;
  struct
  identifier(s)
  identifier(s2)
  =
  {
  const int(3)
  ,
  const int(4)
  }
  ;
  identifier(s1)
  =
  identifier(s2)
  ;
  identifier(s2)
  .
  identifier(x)
  =
  const int(5)
  ;
  return
  identifier(s1)
  .
  identifier(x)
  ;
  }
