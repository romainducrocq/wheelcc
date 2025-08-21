-- Lexing ... OK
+
+
@@ Tokens @@
List[68]:
  struct
  identifier(inner)
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
  identifier(outer)
  {
  int
  identifier(a)
  ;
  struct
  identifier(inner)
  identifier(b)
  ;
  }
  ;
  struct
  identifier(outer)
  identifier(return_struct)
  (
  void
  )
  {
  struct
  identifier(outer)
  identifier(result)
  =
  {
  const int(1)
  ,
  {
  const int(2)
  ,
  const int(3)
  }
  }
  ;
  return
  identifier(result)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  identifier(return_struct)
  (
  )
  .
  identifier(b)
  .
  identifier(x)
  =
  const int(10)
  ;
  return
  const int(0)
  ;
  }
