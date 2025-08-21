-- Lexing ... OK
+
+
@@ Tokens @@
List[29]:
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
