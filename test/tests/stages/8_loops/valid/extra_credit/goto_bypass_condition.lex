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
  identifier(i)
  =
  const int(1)
  ;
  do
  {
  identifier(while_start)
  :
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  ;
  if
  (
  identifier(i)
  <
  const int(10)
  )
  goto
  identifier(while_start)
  ;
  }
  while
  (
  const int(0)
  )
  ;
  return
  identifier(i)
  ;
  }
