-- Lexing ... OK
+
+
@@ Tokens @@
List[90]:
  int
  identifier(target)
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
  identifier(y)
  =
  identifier(x)
  ++
  ;
  int
  identifier(z)
  =
  ++
  identifier(x)
  ;
  int
  identifier(a)
  =
  const int(0)
  ;
  int
  identifier(b)
  =
  --
  identifier(a)
  ;
  int
  identifier(c)
  =
  identifier(a)
  --
  ;
  if
  (
  identifier(x)
  ==
  const int(7)
  &&
  identifier(y)
  ==
  const int(5)
  &&
  identifier(z)
  ==
  const int(7)
  &&
  identifier(a)
  ==
  -
  const int(2)
  &&
  identifier(b)
  ==
  -
  const int(1)
  &&
  identifier(c)
  ==
  -
  const int(1)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  return
  identifier(target)
  (
  )
  ;
  }
