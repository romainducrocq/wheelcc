-- Lexing ... OK
+
+
@@ Tokens @@
List[97]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(100)
  ;
  identifier(x)
  <<=
  const long(22l)
  ;
  if
  (
  identifier(x)
  !=
  const int(419430400)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  (
  identifier(x)
  >>=
  const long(4l)
  )
  !=
  const int(26214400)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(x)
  !=
  const int(26214400)
  )
  {
  return
  const int(3)
  ;
  }
  long
  identifier(l)
  =
  const long(12345l)
  ;
  if
  (
  (
  identifier(l)
  <<=
  const int(33)
  )
  !=
  const long(106042742538240l)
  )
  {
  return
  const int(4)
  ;
  }
  identifier(l)
  =
  -
  identifier(l)
  ;
  if
  (
  (
  identifier(l)
  >>=
  const int(10)
  )
  !=
  -
  const long(103557365760l)
  )
  {
  return
  const int(5)
  ;
  }
  return
  const int(0)
  ;
  }
