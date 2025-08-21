-- Lexing ... OK
+
+
@@ Tokens @@
List[73]:
  int
  identifier(x)
  =
  const int(1)
  ;
  int
  identifier(y)
  =
  const int(0)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  identifier(y)
  =
  -
  const int(1)
  ;
  identifier(x)
  =
  (
  identifier(x)
  <<
  const int(1)
  )
  |
  const int(1)
  ;
  if
  (
  identifier(x)
  !=
  const int(3)
  )
  {
  return
  const int(1)
  ;
  }
  identifier(y)
  =
  (
  (
  identifier(y)
  &
  -
  const int(5)
  )
  ^
  const int(12)
  )
  >>
  const int(2)
  ;
  if
  (
  identifier(y)
  !=
  -
  const int(3)
  )
  {
  return
  const int(2)
  ;
  }
  return
  const int(0)
  ;
  }
