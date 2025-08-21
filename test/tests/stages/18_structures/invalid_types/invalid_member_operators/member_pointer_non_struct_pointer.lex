-- Lexing ... OK
+
+
@@ Tokens @@
List[33]:
  struct
  identifier(a)
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
  struct
  identifier(a)
  identifier(my_struct)
  =
  {
  const int(1)
  ,
  const int(2)
  }
  ;
  return
  identifier(my_struct)
  ->
  identifier(x)
  ;
  }
