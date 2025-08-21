-- Lexing ... OK
+
+
@@ Tokens @@
List[79]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(10)
  ;
  int
  identifier(y)
  =
  const int(0)
  ;
  int
  identifier(z)
  =
  const int(0)
  ;
  do
  {
  identifier(z)
  =
  identifier(z)
  +
  const int(1)
  ;
  if
  (
  identifier(x)
  <=
  const int(0)
  )
  continue
  ;
  identifier(x)
  =
  identifier(x)
  -
  const int(1)
  ;
  if
  (
  identifier(y)
  >=
  const int(10)
  )
  continue
  ;
  identifier(y)
  =
  identifier(y)
  +
  const int(1)
  ;
  }
  while
  (
  identifier(z)
  !=
  const int(50)
  )
  ;
  return
  identifier(z)
  ==
  const int(50)
  &&
  identifier(x)
  ==
  const int(0)
  &&
  identifier(y)
  ==
  const int(10)
  ;
  }
