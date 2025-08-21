-- Lexing ... OK
+
+
@@ Tokens @@
List[194]:
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
  if
  (
  (
  identifier(l1)
  &
  identifier(l2)
  )
  !=
  const long(71777214277877760l)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  (
  identifier(l1)
  |
  identifier(l2)
  )
  !=
  -
  const int(4278255361)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  (
  identifier(l1)
  ^
  identifier(l2)
  )
  !=
  -
  const int(71777218556133121)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  (
  -
  const long(1l)
  &
  const long(34359738368l)
  )
  !=
  const long(34359738368l)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  (
  const long(0l)
  |
  const long(34359738368l)
  )
  !=
  const long(34359738368l)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  (
  const long(34359738368l)
  ^
  const long(137438953472l)
  )
  !=
  const long(171798691840l)
  )
  {
  return
  const int(6)
  ;
  }
  long
  identifier(l)
  =
  const long(4611686018427387903l)
  ;
  int
  identifier(i)
  =
  -
  const int(1073741824)
  ;
  int
  identifier(i2)
  =
  -
  const int(1)
  ;
  if
  (
  (
  identifier(i)
  &
  identifier(l)
  )
  !=
  const long(4611686017353646080l)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  (
  identifier(l)
  |
  identifier(i)
  )
  !=
  -
  const int(1)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  (
  identifier(l)
  ^
  identifier(i)
  )
  !=
  -
  const int(4611686017353646081)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  (
  identifier(i2)
  ^
  const long(4611686018427387903l)
  )
  !=
  ~
  const long(4611686018427387903l)
  )
  {
  return
  const int(10)
  ;
  }
  return
  const int(0)
  ;
  }
