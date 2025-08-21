-- Lexing ... OK
+
+
@@ Tokens @@
List[52]:
  struct
  identifier(inner)
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
  identifier(outer)
  {
  struct
  identifier(inner)
  identifier(x)
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
  struct
  identifier(outer)
  identifier(y)
  =
  {
  const int(1)
  ,
  identifier(x)
  }
  ;
  return
  const int(0)
  ;
  }
