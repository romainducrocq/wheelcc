-- Lexing ... OK
+
+
@@ Tokens @@
List[49]:
  int
  identifier(target)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(100)
  ;
  int
  identifier(y)
  =
  -
  identifier(x)
  *
  const int(3)
  +
  const int(300)
  ;
  return
  (
  identifier(y)
  ?
  identifier(x)
  %
  const int(3)
  :
  identifier(x)
  /
  const int(4)
  )
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
  ==
  const int(25)
  ;
  }
