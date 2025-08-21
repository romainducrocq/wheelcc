-- Lexing ... OK
+
+
@@ Tokens @@
List[137]:
  void
  *
  identifier(malloc)
  (
  unsigned
  long
  identifier(size)
  )
  ;
  void
  identifier(free)
  (
  void
  *
  identifier(ptr)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(d)
  ;
  if
  (
  sizeof
  identifier(d)
  !=
  const int(8)
  )
  {
  return
  const int(2)
  ;
  }
  unsigned
  char
  identifier(c)
  ;
  if
  (
  sizeof
  identifier(c)
  !=
  const int(1)
  )
  {
  return
  const int(3)
  ;
  }
  void
  *
  identifier(buffer)
  =
  identifier(malloc)
  (
  const int(100)
  )
  ;
  if
  (
  sizeof
  (
  identifier(buffer)
  )
  !=
  const int(8)
  )
  {
  return
  const int(4)
  ;
  }
  identifier(free)
  (
  identifier(buffer)
  )
  ;
  if
  (
  sizeof
  (
  (
  int
  )
  identifier(d)
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
  identifier(d)
  ?
  identifier(c)
  :
  const long(10l)
  )
  !=
  const int(8)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  sizeof
  (
  identifier(c)
  =
  const double(10.0)
  )
  !=
  const int(1)
  )
  {
  return
  const int(7)
  ;
  }
  return
  const int(0)
  ;
  }
