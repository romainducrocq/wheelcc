-- Lexing ... OK
+
+
@@ Tokens @@
List[41]:
  long
  identifier(add)
  (
  int
  identifier(a)
  ,
  int
  identifier(b)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  long
  identifier(a)
  =
  identifier(add)
  (
  const int(2147483645)
  ,
  const int(2147483645)
  )
  ;
  if
  (
  identifier(a)
  !=
  const long(4294967290l)
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
