-- Lexing ... OK
+
+
@@ Tokens @@
List[128]:
  unsigned
  int
  identifier(u)
  =
  const long(1152921506754330636l)
  ;
  int
  identifier(i)
  =
  const unsigned int(2147483650u)
  ;
  long
  identifier(l)
  =
  const unsigned int(9223372036854775900u)
  ;
  long
  identifier(l2)
  =
  const unsigned int(2147483650u)
  ;
  unsigned
  long
  identifier(ul)
  =
  const unsigned int(4294967294u)
  ;
  unsigned
  long
  identifier(ul2)
  =
  const long(9223372036854775798l)
  ;
  int
  identifier(i2)
  =
  const unsigned long(9223372039002259606ul)
  ;
  unsigned
  identifier(ui2)
  =
  const unsigned long(9223372039002259606ul)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(u)
  !=
  const unsigned int(2147483660u)
  )
  return
  const int(1)
  ;
  if
  (
  identifier(i)
  !=
  -
  const int(2147483646)
  )
  return
  const int(2)
  ;
  if
  (
  identifier(l)
  !=
  -
  const long(9223372036854775716l)
  )
  return
  const int(3)
  ;
  if
  (
  identifier(l2)
  !=
  const long(2147483650l)
  )
  return
  const int(4)
  ;
  if
  (
  identifier(ul)
  !=
  const unsigned long(4294967294ul)
  )
  return
  const int(5)
  ;
  if
  (
  identifier(ul2)
  !=
  const unsigned long(9223372036854775798ul)
  )
  return
  const int(6)
  ;
  if
  (
  identifier(i2)
  !=
  -
  const int(2147483498)
  )
  return
  const int(7)
  ;
  if
  (
  identifier(ui2)
  !=
  const unsigned int(2147483798u)
  )
  return
  const int(8)
  ;
  return
  const int(0)
  ;
  }
