-- Lexing ... OK
+
+
@@ Tokens @@
List[61]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(acc)
  =
  const int(0)
  ;
  int
  identifier(x)
  =
  const int(100)
  ;
  while
  (
  identifier(x)
  )
  {
  int
  identifier(y)
  =
  const int(10)
  ;
  identifier(x)
  =
  identifier(x)
  -
  identifier(y)
  ;
  while
  (
  identifier(y)
  )
  {
  identifier(acc)
  =
  identifier(acc)
  +
  const int(1)
  ;
  identifier(y)
  =
  identifier(y)
  -
  const int(1)
  ;
  }
  }
  return
  identifier(acc)
  ==
  const int(100)
  &&
  identifier(x)
  ==
  const int(0)
  ;
  }
