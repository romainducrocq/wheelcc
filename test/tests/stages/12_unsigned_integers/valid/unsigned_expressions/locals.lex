-- Lexing ... OK
+
+
@@ Tokens @@
List[148]:
  int
  identifier(main)
  (
  void
  )
  {
  unsigned
  long
  identifier(a)
  =
  const unsigned long(8589934592ul)
  ;
  int
  identifier(b)
  =
  -
  const int(1)
  ;
  long
  identifier(c)
  =
  -
  const long(8589934592l)
  ;
  unsigned
  int
  identifier(d)
  =
  const unsigned int(10u)
  ;
  if
  (
  identifier(a)
  !=
  const unsigned long(8589934592ul)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(b)
  !=
  -
  const int(1)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(c)
  !=
  -
  const long(8589934592l)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(d)
  !=
  const unsigned int(10u)
  )
  {
  return
  const int(4)
  ;
  }
  identifier(a)
  =
  -
  identifier(a)
  ;
  identifier(b)
  =
  identifier(b)
  -
  const int(1)
  ;
  identifier(c)
  =
  identifier(c)
  +
  const long(8589934594l)
  ;
  identifier(d)
  =
  identifier(d)
  *
  const unsigned int(268435456u)
  ;
  if
  (
  identifier(a)
  !=
  const unsigned long(18446744065119617024ul)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(b)
  !=
  -
  const int(2)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(c)
  !=
  const int(2)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(d)
  !=
  const unsigned int(2684354560u)
  )
  {
  return
  const int(8)
  ;
  }
  return
  const int(0)
  ;
  }
