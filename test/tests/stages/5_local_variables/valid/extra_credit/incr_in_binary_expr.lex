-- Lexing ... OK
+
+
@@ Tokens @@
List[43]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(a)
  =
  const int(2)
  ;
  int
  identifier(b)
  =
  const int(3)
  +
  identifier(a)
  ++
  ;
  int
  identifier(c)
  =
  const int(4)
  +
  ++
  identifier(b)
  ;
  return
  (
  identifier(a)
  ==
  const int(3)
  &&
  identifier(b)
  ==
  const int(6)
  &&
  identifier(c)
  ==
  const int(10)
  )
  ;
  }
