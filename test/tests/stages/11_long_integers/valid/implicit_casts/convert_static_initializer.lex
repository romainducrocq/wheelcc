-- Lexing ... OK
+
+
@@ Tokens @@
List[42]:
  int
  identifier(i)
  =
  const long(8589934592l)
  ;
  long
  identifier(j)
  =
  const int(123456)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(i)
  !=
  const int(0)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(j)
  !=
  const long(123456l)
  )
  {
  return
  const int(2)
  ;
  }
  return
  const int(0)
  ;
  }
