-- Lexing ... OK
+
+
@@ Tokens @@
List[201]:
  double
  identifier(d1)
  =
  const int(2147483647)
  ;
  double
  identifier(d2)
  =
  const unsigned int(4294967295u)
  ;
  double
  identifier(d3)
  =
  const long(4611686018427389440l)
  ;
  double
  identifier(d4)
  =
  const long(4611686018427389955l)
  ;
  double
  identifier(d5)
  =
  const unsigned long(9223372036854775810ul)
  ;
  double
  identifier(d6)
  =
  const unsigned long(4611686018427389955ul)
  ;
  double
  identifier(d7)
  =
  const unsigned long(9223372036854776832ul)
  ;
  double
  identifier(uninitialized)
  ;
  static
  int
  identifier(i)
  =
  const double(4.9)
  ;
  int
  unsigned
  identifier(u)
  =
  const double(42949.672923E5)
  ;
  long
  identifier(l)
  =
  const double(4611686018427389440.)
  ;
  unsigned
  long
  identifier(ul)
  =
  const double(18446744073709549568.)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(d1)
  !=
  const double(2147483647.)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(d2)
  !=
  const double(4294967295.)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(d3)
  !=
  const double(4611686018427389952.)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(d4)
  !=
  identifier(d3)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(d5)
  !=
  const double(9223372036854775808.)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(d6)
  !=
  identifier(d3)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(d7)
  !=
  identifier(d5)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(uninitialized)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(i)
  !=
  const int(4)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(u)
  !=
  const unsigned int(4294967292u)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  identifier(l)
  !=
  const long(4611686018427389952l)
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  identifier(ul)
  !=
  const unsigned long(18446744073709549568ul)
  )
  {
  return
  const int(12)
  ;
  }
  return
  const int(0)
  ;
  }
