-- Lexing ... OK
+
+
@@ Tokens @@
List[670]:
  int
  identifier(target_chars)
  (
  void
  )
  {
  signed
  char
  identifier(c1)
  =
  -
  const int(128)
  ;
  signed
  char
  identifier(c2)
  =
  -
  const int(120)
  ;
  signed
  char
  identifier(c3)
  =
  -
  const int(2)
  ;
  signed
  char
  identifier(c4)
  =
  const int(1)
  ;
  signed
  char
  identifier(c5)
  =
  const int(120)
  ;
  unsigned
  char
  identifier(u1)
  =
  const int(0)
  ;
  unsigned
  char
  identifier(u2)
  =
  const int(170)
  ;
  unsigned
  char
  identifier(u3)
  =
  const int(250)
  ;
  unsigned
  char
  identifier(u4)
  =
  const int(255)
  ;
  identifier(c1)
  ^=
  const int(12345)
  ;
  identifier(c2)
  |=
  identifier(u4)
  ;
  identifier(c3)
  &=
  identifier(u2)
  -
  (
  unsigned
  char
  )
  const int(185)
  ;
  identifier(c4)
  <<=
  const unsigned int(7u)
  ;
  identifier(c5)
  >>=
  const int(31)
  ;
  long
  identifier(x)
  =
  const int(32)
  ;
  identifier(u4)
  <<=
  const int(12)
  ;
  identifier(u3)
  >>=
  (
  identifier(x)
  -
  const int(1)
  )
  ;
  identifier(u2)
  |=
  -
  const int(399)
  ;
  identifier(x)
  =
  -
  const long(4296140120l)
  ;
  identifier(u1)
  ^=
  identifier(x)
  ;
  if
  (
  identifier(c1)
  !=
  -
  const int(71)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(c2)
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
  identifier(c3)
  !=
  -
  const int(16)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(c4)
  !=
  -
  const int(128)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(c5)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(u1)
  !=
  const int(168)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(u2)
  !=
  const int(251)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(u3)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(u4)
  )
  {
  return
  const int(9)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(target_long_bitwise)
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
  int
  identifier(target_long_bitshift)
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
  int
  identifier(target_unsigned_bitwise)
  (
  void
  )
  {
  unsigned
  long
  identifier(ul)
  =
  const unsigned long(18446460386757245432ul)
  ;
  identifier(ul)
  &=
  -
  const int(1000)
  ;
  if
  (
  identifier(ul)
  !=
  const unsigned long(18446460386757244952ul)
  )
  {
  return
  const int(1)
  ;
  }
  identifier(ul)
  |=
  const unsigned int(4294967040u)
  ;
  if
  (
  identifier(ul)
  !=
  const unsigned long(18446460386824683288ul)
  )
  {
  return
  const int(2)
  ;
  }
  int
  identifier(i)
  =
  const int(123456)
  ;
  unsigned
  int
  identifier(ui)
  =
  const unsigned int(4042322160u)
  ;
  long
  identifier(l)
  =
  -
  const int(252645136)
  ;
  if
  (
  identifier(ui)
  ^=
  identifier(l)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(ui)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(i)
  !=
  const int(123456)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(l)
  !=
  -
  const int(252645136)
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
  int
  identifier(target_unsigned_bitshift)
  (
  void
  )
  {
  int
  identifier(i)
  =
  -
  const int(2)
  ;
  identifier(i)
  >>=
  const unsigned int(3u)
  ;
  if
  (
  identifier(i)
  !=
  -
  const int(1)
  )
  {
  return
  const int(1)
  ;
  }
  unsigned
  long
  identifier(ul)
  =
  const unsigned long(18446744073709551615UL)
  ;
  identifier(ul)
  <<=
  const int(44)
  ;
  if
  (
  identifier(ul)
  !=
  const unsigned long(18446726481523507200ul)
  )
  {
  return
  const int(2)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(target_chars)
  (
  )
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(target_long_bitwise)
  (
  )
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_long_bitshift)
  (
  )
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_unsigned_bitwise)
  (
  )
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(target_unsigned_bitshift)
  (
  )
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
