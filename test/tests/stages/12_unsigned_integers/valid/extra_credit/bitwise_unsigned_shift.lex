-- Lexing ... OK
+
+
@@ Tokens @@
List[83]:
  int
  identifier(main)
  (
  void
  )
  {
  unsigned
  int
  identifier(ui)
  =
  -
  const unsigned int(1u)
  ;
  if
  (
  (
  identifier(ui)
  <<
  const long(2l)
  )
  !=
  const int(4294967292)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  (
  identifier(ui)
  >>
  const int(2)
  )
  !=
  const int(1073741823)
  )
  {
  return
  const int(2)
  ;
  }
  static
  int
  identifier(shiftcount)
  =
  const int(5)
  ;
  if
  (
  (
  const unsigned int(1000000u)
  >>
  identifier(shiftcount)
  )
  !=
  const int(31250)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  (
  const unsigned int(1000000u)
  <<
  identifier(shiftcount)
  )
  !=
  const int(32000000)
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
