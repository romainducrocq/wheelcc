-- Lexing ... OK
+
+
@@ Tokens @@
List[123]:
  int
  identifier(main)
  (
  void
  )
  {
  static
  long
  identifier(l)
  =
  const int(0)
  ;
  int
  identifier(i)
  =
  const int(0)
  ;
  static
  char
  identifier(c)
  =
  const int(0)
  ;
  if
  (
  sizeof
  (
  identifier(c)
  &
  identifier(i)
  )
  !=
  const int(4)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  sizeof
  (
  identifier(i)
  |
  identifier(l)
  )
  !=
  const int(8)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  sizeof
  (
  identifier(c)
  ^
  identifier(c)
  )
  !=
  const int(4)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  sizeof
  (
  identifier(i)
  <<
  identifier(l)
  )
  !=
  const int(4)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  sizeof
  (
  identifier(c)
  <<
  identifier(i)
  )
  !=
  const int(4)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  sizeof
  (
  identifier(l)
  >>
  identifier(c)
  )
  !=
  const int(8)
  )
  {
  return
  const int(6)
  ;
  }
  return
  const int(0)
  ;
  }
