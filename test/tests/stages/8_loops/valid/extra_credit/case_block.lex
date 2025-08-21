-- Lexing ... OK
+
+
@@ Tokens @@
List[48]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(a)
  =
  const int(4)
  ;
  int
  identifier(b)
  =
  const int(0)
  ;
  switch
  (
  const int(2)
  )
  {
  case
  const int(2)
  :
  {
  int
  identifier(a)
  =
  const int(8)
  ;
  identifier(b)
  =
  identifier(a)
  ;
  }
  }
  return
  (
  identifier(a)
  ==
  const int(4)
  &&
  identifier(b)
  ==
  const int(8)
  )
  ;
  }
