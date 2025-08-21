-- Lexing ... OK
+
+
@@ Tokens @@
List[147]:
  int
  identifier(main)
  (
  void
  )
  {
  long
  identifier(a)
  =
  const long(8589934592l)
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
  int
  identifier(d)
  =
  const int(10)
  ;
  if
  (
  identifier(a)
  !=
  const long(8589934592l)
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
  const int(10)
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
  +
  const int(10)
  ;
  if
  (
  identifier(a)
  !=
  -
  const long(8589934592l)
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
  const int(20)
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
