-- Lexing ... OK
+
+
@@ Tokens @@
List[100]:
  int
  identifier(putchar)
  (
  int
  identifier(c)
  )
  ;
  int
  identifier(target)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(5)
  ;
  int
  identifier(y)
  =
  const int(65)
  ;
  do
  {
  identifier(x)
  =
  identifier(y)
  +
  const int(2)
  ;
  if
  (
  identifier(y)
  >
  const int(70)
  )
  {
  identifier(x)
  =
  identifier(y)
  +
  const int(3)
  ;
  }
  identifier(y)
  =
  identifier(putchar)
  (
  identifier(x)
  )
  +
  const int(3)
  ;
  }
  while
  (
  identifier(y)
  <
  const int(90)
  )
  ;
  if
  (
  identifier(x)
  !=
  const int(90)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(y)
  !=
  const int(93)
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
  int
  identifier(main)
  (
  void
  )
  {
  return
  identifier(target)
  (
  )
  ;
  }
