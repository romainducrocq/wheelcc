-- Lexing ... OK
+
+
@@ Tokens @@
List[105]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(i)
  =
  -
  const int(20)
  ;
  int
  identifier(b)
  =
  const int(2147483647)
  ;
  int
  identifier(c)
  =
  -
  const int(5000000)
  ;
  identifier(i)
  +=
  const long(2147483648l)
  ;
  if
  (
  identifier(i)
  !=
  const int(2147483628)
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
  const int(2147483647)
  )
  {
  return
  const int(2)
  ;
  }
  identifier(b)
  /=
  -
  const long(34359738367l)
  ;
  if
  (
  identifier(b)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(i)
  !=
  const int(2147483628)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(c)
  !=
  -
  const int(5000000)
  )
  {
  return
  const int(5)
  ;
  }
  identifier(c)
  *=
  const long(10000l)
  ;
  if
  (
  identifier(c)
  !=
  const int(1539607552)
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
