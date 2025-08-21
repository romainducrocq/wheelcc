-- Lexing ... OK
+
+
@@ Tokens @@
List[47]:
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
  if
  (
  identifier(x)
  )
  {
  identifier(x)
  =
  const int(5)
  ;
  goto
  identifier(l)
  ;
  return
  const int(0)
  ;
  identifier(l)
  :
  return
  identifier(x)
  ;
  }
  else
  {
  goto
  identifier(l)
  ;
  return
  const int(0)
  ;
  identifier(l)
  :
  return
  identifier(x)
  ;
  }
  }
