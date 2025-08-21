-- Lexing ... OK
+
+
@@ Tokens @@
List[127]:
  int
  identifier(main)
  (
  void
  )
  {
  long
  identifier(l1)
  =
  const long(71777214294589695l)
  ;
  long
  identifier(l2)
  =
  -
  const int(4294967296)
  ;
  identifier(l1)
  &=
  identifier(l2)
  ;
  if
  (
  identifier(l1)
  !=
  const long(71777214277877760l)
  )
  {
  return
  const int(1)
  ;
  }
  identifier(l2)
  |=
  const long(100l)
  ;
  if
  (
  identifier(l2)
  !=
  -
  const int(4294967196)
  )
  {
  return
  const int(2)
  ;
  }
  identifier(l1)
  ^=
  -
  const long(9223372036854775807l)
  ;
  if
  (
  identifier(l1)
  !=
  -
  const long(9151594822576898047l)
  )
  {
  return
  const int(3)
  ;
  }
  identifier(l1)
  =
  const long(4611686018427387903l)
  ;
  int
  identifier(i)
  =
  -
  const int(1073741824)
  ;
  identifier(l1)
  &=
  identifier(i)
  ;
  if
  (
  identifier(l1)
  !=
  const long(4611686017353646080l)
  )
  {
  return
  const int(4)
  ;
  }
  identifier(i)
  =
  -
  const long(2147483648l)
  ;
  if
  (
  (
  identifier(i)
  |=
  const long(71777214294589695l)
  )
  !=
  -
  const int(2130771713)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(i)
  !=
  -
  const int(2130771713)
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
