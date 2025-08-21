-- Lexing ... OK
+
+
@@ Tokens @@
List[67]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(5)
  ;
  int
  identifier(acc)
  =
  const int(0)
  ;
  while
  (
  identifier(x)
  >=
  const int(0)
  )
  {
  int
  identifier(i)
  =
  identifier(x)
  ;
  while
  (
  identifier(i)
  <=
  const int(10)
  )
  {
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  ;
  if
  (
  identifier(i)
  %
  const int(2)
  )
  continue
  ;
  identifier(acc)
  =
  identifier(acc)
  +
  const int(1)
  ;
  }
  identifier(x)
  =
  identifier(x)
  -
  const int(1)
  ;
  }
  return
  identifier(acc)
  ;
  }
