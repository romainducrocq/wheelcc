-- Lexing ... OK
+
+
@@ Tokens @@
List[50]:
  union
  identifier(u)
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
  union
  identifier(u)
  identifier(my_union)
  =
  {
  const int(0)
  }
  ;
  identifier(my_union)
  .
  identifier(a)
  =
  -
  const int(1)
  ;
  if
  (
  identifier(my_union)
  .
  identifier(b)
  !=
  -
  const int(1)
  )
  {
  return
  const int(1)
  ;
  }
  return
  const int(0)
  ;
  }
