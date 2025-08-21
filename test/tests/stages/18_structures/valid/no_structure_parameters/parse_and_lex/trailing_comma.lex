-- Lexing ... OK
+
+
@@ Tokens @@
List[51]:
  struct
  identifier(s)
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
  identifier(s)
  identifier(x)
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  }
  ;
  if
  (
  identifier(x)
  .
  identifier(a)
  !=
  const int(1)
  ||
  identifier(x)
  .
  identifier(b)
  !=
  const int(2)
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
