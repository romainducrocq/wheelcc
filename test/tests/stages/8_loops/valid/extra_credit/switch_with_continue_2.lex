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
  identifier(sum)
  =
  const int(0)
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(10)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  switch
  (
  identifier(i)
  %
  const int(2)
  )
  {
  case
  const int(0)
  :
  continue
  ;
  default
  :
  identifier(sum)
  =
  identifier(sum)
  +
  const int(1)
  ;
  }
  }
  return
  identifier(sum)
  ;
  }
