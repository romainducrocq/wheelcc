-- Lexing ... OK
+
+
@@ Tokens @@
List[55]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(a)
  =
  const int(0)
  ;
  if
  (
  identifier(a)
  )
  {
  int
  identifier(b)
  =
  const int(2)
  ;
  return
  identifier(b)
  ;
  }
  else
  {
  int
  identifier(c)
  =
  const int(3)
  ;
  if
  (
  identifier(a)
  <
  identifier(c)
  )
  {
  return
  !
  identifier(a)
  ;
  }
  else
  {
  return
  const int(5)
  ;
  }
  }
  return
  identifier(a)
  ;
  }
