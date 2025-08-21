-- Lexing ... OK
+
+
@@ Tokens @@
List[48]:
  struct
  identifier(one)
  {
  int
  identifier(x)
  ;
  int
  identifier(y)
  ;
  }
  ;
  struct
  identifier(two)
  {
  int
  identifier(a)
  ;
  int
  identifier(b)
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
  identifier(one)
  identifier(x)
  =
  {
  const int(1)
  ,
  const int(2)
  }
  ;
  struct
  identifier(two)
  identifier(y)
  =
  identifier(x)
  ;
  return
  const int(0)
  ;
  }
