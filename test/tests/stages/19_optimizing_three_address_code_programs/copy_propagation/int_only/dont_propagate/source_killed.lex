-- Lexing ... OK
+
+
@@ Tokens @@
List[46]:
  int
  identifier(x)
  =
  const int(10)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(y)
  =
  identifier(x)
  ;
  identifier(x)
  =
  const int(4)
  ;
  if
  (
  identifier(y)
  !=
  const int(10)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(x)
  !=
  const int(4)
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
