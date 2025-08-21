-- Lexing ... OK
+
+
@@ Tokens @@
List[105]:
  union
  identifier(u)
  {
  double
  identifier(d)
  ;
  long
  identifier(l)
  ;
  unsigned
  long
  identifier(ul)
  ;
  char
  identifier(c)
  ;
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  union
  identifier(u)
  identifier(x)
  =
  {
  const int(20)
  }
  ;
  if
  (
  identifier(x)
  .
  identifier(d)
  !=
  const double(20.0)
  )
  {
  return
  const int(1)
  ;
  }
  union
  identifier(u)
  *
  identifier(ptr)
  =
  &
  identifier(x)
  ;
  identifier(ptr)
  ->
  identifier(l)
  =
  -
  const long(1l)
  ;
  if
  (
  identifier(ptr)
  ->
  identifier(l)
  !=
  -
  const long(1l)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(ptr)
  ->
  identifier(ul)
  !=
  const unsigned long(18446744073709551615UL)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(x)
  .
  identifier(c)
  !=
  -
  const int(1)
  )
  {
  return
  const int(4)
  ;
  }
  return
  const int(0)
  ;
  }
