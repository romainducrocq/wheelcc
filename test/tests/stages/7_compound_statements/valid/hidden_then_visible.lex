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
  ;
  {
  identifier(a)
  =
  -
  const int(4)
  ;
  int
  identifier(a)
  =
  const int(7)
  ;
  identifier(b)
  =
  identifier(a)
  +
  const int(1)
  ;
  }
  return
  identifier(b)
  ==
  const int(8)
  &&
  identifier(a)
  ==
  -
  const int(4)
  ;
  }
