-- Lexing ... OK
+
+
@@ Tokens @@
List[38]:
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
  union
  identifier(u)
  identifier(my_union)
  =
  {
  -
  const int(1)
  }
  ;
  identifier(my_union)
  .
  identifier(l)
  =
  const int(180)
  ;
  return
  identifier(my_union)
  .
  identifier(i)
  ;
  }
