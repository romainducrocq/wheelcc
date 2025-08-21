-- Lexing ... OK
+
+
@@ Tokens @@
List[113]:
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
  int
  identifier(a)
  ,
  int
  identifier(b)
  ,
  int
  identifier(c)
  ,
  int
  identifier(d)
  )
  {
  while
  (
  identifier(a)
  >
  const int(0)
  )
  {
  while
  (
  identifier(c)
  >
  const int(0)
  )
  {
  identifier(putchar)
  (
  identifier(c)
  +
  identifier(d)
  )
  ;
  identifier(c)
  =
  identifier(c)
  -
  const int(1)
  ;
  if
  (
  identifier(d)
  %
  const int(2)
  )
  {
  identifier(c)
  =
  identifier(c)
  -
  const int(2)
  ;
  }
  }
  while
  (
  identifier(b)
  >
  const int(0)
  )
  {
  identifier(c)
  =
  const int(10)
  ;
  identifier(b)
  =
  identifier(b)
  -
  const int(1)
  ;
  }
  identifier(a)
  =
  identifier(a)
  -
  const int(1)
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
  const int(5)
  ,
  const int(4)
  ,
  const int(3)
  ,
  const int(65)
  )
  ;
  }
