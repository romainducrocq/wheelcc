-- Lexing ... OK
+
+
@@ Tokens @@
List[49]:
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
  )
  {
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  ;
  if
  (
  identifier(i)
  %
  const int(2)
  )
  continue
  ;
  identifier(sum)
  =
  identifier(sum)
  +
  identifier(i)
  ;
  }
  return
  identifier(sum)
  ;
  }
