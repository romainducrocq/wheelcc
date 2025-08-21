-- Lexing ... OK
+
+
@@ Tokens @@
List[146]:
  int
  identifier(main)
  (
  void
  )
  {
  long
  identifier(l)
  =
  const long(137438953472l)
  ;
  int
  identifier(shiftcount)
  =
  const int(2)
  ;
  if
  (
  identifier(l)
  >>
  identifier(shiftcount)
  !=
  const long(34359738368l)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(l)
  <<
  identifier(shiftcount)
  !=
  const int(549755813888)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(l)
  <<
  const int(2)
  !=
  const int(549755813888)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  (
  const long(40l)
  <<
  const int(40)
  )
  !=
  const long(43980465111040l)
  )
  {
  return
  const int(4)
  ;
  }
  long
  identifier(long_shiftcount)
  =
  const long(3l)
  ;
  int
  identifier(i_neighbor1)
  =
  const int(0)
  ;
  int
  identifier(i)
  =
  -
  const int(2147483645)
  ;
  int
  identifier(i_neighbor2)
  =
  const int(0)
  ;
  if
  (
  identifier(i)
  >>
  identifier(long_shiftcount)
  !=
  -
  const int(268435456)
  )
  {
  return
  const int(5)
  ;
  }
  identifier(i)
  =
  -
  const int(1)
  ;
  if
  (
  identifier(i)
  >>
  const long(10l)
  !=
  -
  const int(1)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(i_neighbor1)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(i_neighbor2)
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
