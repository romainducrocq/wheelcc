-- Lexing ... OK
+
+
@@ Tokens @@
List[54]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(a)
  =
  const int(3)
  ;
  int
  identifier(b)
  =
  const int(0)
  ;
  switch
  (
  identifier(a)
  )
  {
  int
  identifier(a)
  =
  (
  identifier(b)
  =
  const int(5)
  )
  ;
  case
  const int(3)
  :
  identifier(a)
  =
  const int(4)
  ;
  identifier(b)
  =
  identifier(b)
  +
  identifier(a)
  ;
  }
  return
  identifier(a)
  ==
  const int(3)
  &&
  identifier(b)
  ==
  const int(4)
  ;
  }
