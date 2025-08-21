-- Lexing ... OK
+
+
@@ Tokens @@
List[40]:
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
  identifier(y)
  =
  (
  identifier(x)
  =
  const int(5)
  )
  ?
  identifier(x)
  :
  const int(2)
  ;
  return
  (
  identifier(x)
  ==
  const int(5)
  &&
  identifier(y)
  ==
  const int(5)
  )
  ;
  }
