-- Lexing ... OK
+
+
@@ Tokens @@
List[52]:
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
  const int(0)
  ;
  if
  (
  identifier(a)
  )
  {
  int
  identifier(a)
  =
  const int(1)
  ;
  identifier(b)
  =
  identifier(a)
  ;
  goto
  identifier(end)
  ;
  }
  identifier(a)
  =
  const int(9)
  ;
  identifier(end)
  :
  return
  (
  identifier(a)
  ==
  const int(10)
  &&
  identifier(b)
  ==
  const int(1)
  )
  ;
  }
