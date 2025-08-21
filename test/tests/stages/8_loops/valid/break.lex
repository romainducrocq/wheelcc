-- Lexing ... OK
+
+
@@ Tokens @@
List[60]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(a)
  =
  const int(10)
  ;
  int
  identifier(b)
  =
  const int(20)
  ;
  for
  (
  identifier(b)
  =
  -
  const int(20)
  ;
  identifier(b)
  <
  const int(0)
  ;
  identifier(b)
  =
  identifier(b)
  +
  const int(1)
  )
  {
  identifier(a)
  =
  identifier(a)
  -
  const int(1)
  ;
  if
  (
  identifier(a)
  <=
  const int(0)
  )
  break
  ;
  }
  return
  identifier(a)
  ==
  const int(0)
  &&
  identifier(b)
  ==
  -
  const int(11)
  ;
  }
