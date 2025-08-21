-- Lexing ... OK
+
+
@@ Tokens @@
List[51]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(shadow)
  =
  const int(1)
  ;
  int
  identifier(acc)
  =
  const int(0)
  ;
  for
  (
  int
  identifier(shadow)
  =
  const int(0)
  ;
  identifier(shadow)
  <
  const int(10)
  ;
  identifier(shadow)
  =
  identifier(shadow)
  +
  const int(1)
  )
  {
  identifier(acc)
  =
  identifier(acc)
  +
  identifier(shadow)
  ;
  }
  return
  identifier(acc)
  ==
  const int(45)
  &&
  identifier(shadow)
  ==
  const int(1)
  ;
  }
