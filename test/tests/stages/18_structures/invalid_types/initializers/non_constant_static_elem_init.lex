-- Lexing ... OK
+
+
@@ Tokens @@
List[43]:
  struct
  identifier(pair)
  {
  int
  identifier(a)
  ;
  int
  identifier(b)
  ;
  }
  ;
  struct
  identifier(pair)
  identifier(x)
  =
  {
  const int(1)
  ,
  const int(2)
  }
  ;
  struct
  identifier(outer)
  {
  double
  identifier(d)
  ;
  struct
  identifier(pair)
  identifier(inner)
  ;
  }
  ;
  struct
  identifier(outer)
  identifier(y)
  =
  {
  const double(1.0)
  ,
  identifier(x)
  }
  ;
