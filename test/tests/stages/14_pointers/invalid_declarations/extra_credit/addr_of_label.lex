-- Lexing ... OK
+
+
@@ Tokens @@
List[36]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(0)
  ;
  identifier(lbl)
  :
  identifier(x)
  =
  const int(1)
  ;
  if
  (
  &
  identifier(lbl)
  ==
  const int(0)
  )
  {
  return
  const int(1)
  ;
  }
  goto
  identifier(lbl)
  ;
  return
  const int(0)
  ;
  }
