-- Lexing ... OK
+
+
@@ Tokens @@
List[1154]:
  long
  identifier(target_extend_int_to_long)
  (
  void
  )
  {
  int
  identifier(i)
  =
  -
  const int(1000)
  ;
  long
  identifier(l)
  =
  (
  long
  )
  identifier(i)
  ;
  return
  (
  identifier(l)
  -
  const long(72057594037927936l)
  )
  /
  const long(3l)
  ;
  }
  unsigned
  long
  identifier(target_extend_int_to_ulong)
  (
  void
  )
  {
  int
  identifier(i)
  =
  -
  const int(1000)
  ;
  unsigned
  long
  identifier(u)
  =
  (
  unsigned
  long
  )
  identifier(i)
  ;
  return
  identifier(u)
  %
  const unsigned long(50ul)
  ;
  }
  long
  identifier(target_extend_uint_to_long)
  (
  void
  )
  {
  unsigned
  int
  identifier(u)
  =
  const unsigned int(2147483648u)
  ;
  long
  identifier(l)
  =
  (
  long
  )
  identifier(u)
  ;
  if
  (
  identifier(l)
  <
  const int(0)
  )
  {
  return
  const int(0)
  ;
  }
  return
  identifier(l)
  %
  const long(7l)
  ;
  }
  unsigned
  long
  identifier(target_extend_uint_to_ulong)
  (
  void
  )
  {
  unsigned
  int
  identifier(u)
  =
  const unsigned int(4294967295U)
  ;
  unsigned
  long
  identifier(l)
  =
  (
  unsigned
  long
  )
  identifier(u)
  ;
  return
  (
  identifier(l)
  ==
  const unsigned long(4294967295Ul)
  )
  ;
  }
  int
  identifier(target_truncate_long_to_int)
  (
  void
  )
  {
  long
  identifier(l)
  =
  const long(9223372036854775807l)
  ;
  int
  identifier(i)
  =
  (
  int
  )
  identifier(l)
  ;
  long
  identifier(l2)
  =
  -
  const long(9223372036854775807l)
  -
  const long(1l)
  ;
  int
  identifier(i2)
  =
  (
  int
  )
  identifier(l2)
  ;
  if
  (
  identifier(i2)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(20)
  /
  identifier(i)
  ;
  }
  unsigned
  int
  identifier(target_truncate_long_to_uint)
  (
  void
  )
  {
  long
  identifier(l)
  =
  -
  const long(9223372032559808513l)
  ;
  unsigned
  int
  identifier(u)
  =
  (
  unsigned
  )
  identifier(l)
  ;
  if
  (
  identifier(u)
  -
  const unsigned int(4294967295U)
  )
  {
  return
  const int(0)
  ;
  }
  return
  identifier(u)
  /
  const int(20)
  ;
  }
  int
  identifier(target_truncate_ulong_to_int)
  (
  void
  )
  {
  unsigned
  long
  identifier(ul)
  =
  const unsigned long(18446744073709551615UL)
  ;
  int
  identifier(i)
  =
  (
  int
  )
  identifier(ul)
  ;
  unsigned
  long
  identifier(ul2)
  =
  const unsigned long(9223372039002259456ul)
  ;
  int
  identifier(i2)
  =
  (
  int
  )
  identifier(ul2)
  ;
  if
  (
  identifier(i2)
  >=
  const int(0)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(10)
  /
  identifier(i)
  ;
  }
  unsigned
  int
  identifier(target_truncate_ulong_to_uint)
  (
  void
  )
  {
  unsigned
  long
  identifier(ul)
  =
  const unsigned long(18446744073709551615UL)
  ;
  unsigned
  int
  identifier(u)
  =
  (
  unsigned
  int
  )
  identifier(ul)
  ;
  return
  identifier(u)
  /
  const int(20)
  ;
  }
  int
  identifier(target_char_int_conversion)
  (
  void
  )
  {
  int
  identifier(i)
  =
  const int(257)
  ;
  char
  identifier(c)
  =
  identifier(i)
  ;
  identifier(i)
  =
  const int(255)
  ;
  char
  identifier(c2)
  =
  identifier(i)
  ;
  identifier(i)
  =
  const int(2147483647)
  ;
  signed
  char
  identifier(c3)
  =
  identifier(i)
  ;
  identifier(i)
  =
  -
  const int(2147483647)
  -
  const int(1)
  ;
  char
  identifier(c4)
  =
  identifier(i)
  ;
  identifier(i)
  =
  -
  const int(129)
  ;
  signed
  char
  identifier(c5)
  =
  identifier(i)
  ;
  identifier(i)
  =
  const int(128)
  ;
  char
  identifier(c6)
  =
  identifier(i)
  ;
  if
  (
  identifier(c)
  !=
  const int(1)
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
  const int(1)
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
  const int(0)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(c5)
  !=
  const int(127)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(c6)
  !=
  -
  const int(128)
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
  identifier(target_uchar_int_conversion)
  (
  void
  )
  {
  int
  identifier(i)
  =
  const int(767)
  ;
  unsigned
  char
  identifier(uc1)
  =
  identifier(i)
  ;
  identifier(i)
  =
  const int(512)
  ;
  unsigned
  char
  identifier(uc2)
  =
  identifier(i)
  ;
  identifier(i)
  =
  -
  const int(2147483647)
  ;
  unsigned
  char
  identifier(uc3)
  =
  identifier(i)
  ;
  identifier(i)
  =
  -
  const int(2147483647)
  +
  const int(127)
  ;
  unsigned
  char
  identifier(uc4)
  =
  identifier(i)
  ;
  if
  (
  identifier(uc1)
  !=
  const int(255)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(uc2)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(uc3)
  !=
  const int(1)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(uc4)
  !=
  const int(128)
  )
  {
  return
  const int(1)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(target_char_uint_conversion)
  (
  void
  )
  {
  char
  identifier(c)
  =
  const unsigned int(2148532223u)
  ;
  signed
  char
  identifier(c2)
  =
  const unsigned int(2147483775u)
  ;
  unsigned
  int
  identifier(u)
  =
  (
  unsigned
  int
  )
  identifier(c)
  ;
  if
  (
  identifier(u)
  !=
  const unsigned int(4294967295U)
  )
  {
  return
  const int(1)
  ;
  }
  identifier(u)
  =
  (
  unsigned
  int
  )
  identifier(c2)
  ;
  if
  (
  identifier(u)
  !=
  const unsigned int(127u)
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
  identifier(target_uchar_uint_conversion)
  (
  void
  )
  {
  unsigned
  char
  identifier(uc)
  =
  const unsigned int(2148532223u)
  ;
  unsigned
  int
  identifier(ui)
  =
  (
  unsigned
  int
  )
  identifier(uc)
  ;
  if
  (
  identifier(ui)
  !=
  const unsigned int(255u)
  )
  {
  return
  const int(1)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(target_char_long_conversion)
  (
  void
  )
  {
  long
  identifier(l)
  =
  const long(3377699720528001l)
  ;
  char
  identifier(c)
  =
  identifier(l)
  ;
  identifier(l)
  =
  const long(9223372036854775807l)
  ;
  char
  identifier(c2)
  =
  identifier(l)
  ;
  identifier(l)
  =
  const long(2147483648l)
  +
  const long(127l)
  ;
  signed
  char
  identifier(c3)
  =
  identifier(l)
  ;
  identifier(l)
  =
  -
  const long(2147483647l)
  -
  const long(1l)
  ;
  char
  identifier(c4)
  =
  identifier(l)
  ;
  identifier(l)
  =
  const long(2147483648l)
  +
  const long(128l)
  ;
  signed
  char
  identifier(c5)
  =
  identifier(l)
  ;
  if
  (
  identifier(c)
  !=
  -
  const long(127l)
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
  const long(1l)
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
  const long(127l)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(c4)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(c5)
  !=
  -
  const long(128l)
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
  identifier(target_uchar_long_conversion)
  (
  void
  )
  {
  long
  identifier(l)
  =
  const long(255l)
  +
  const long(4294967296l)
  ;
  unsigned
  char
  identifier(uc1)
  =
  identifier(l)
  ;
  identifier(l)
  =
  const long(36028798092705792l)
  ;
  unsigned
  char
  identifier(uc2)
  =
  identifier(l)
  ;
  identifier(l)
  =
  -
  const long(9223372036854775807l)
  ;
  unsigned
  char
  identifier(uc3)
  =
  identifier(l)
  ;
  identifier(l)
  =
  -
  const long(9223372036854775807l)
  +
  const int(127)
  ;
  unsigned
  char
  identifier(uc4)
  =
  identifier(l)
  ;
  if
  (
  identifier(uc1)
  !=
  const int(255)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(uc2)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(uc3)
  !=
  const int(1)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(uc4)
  !=
  const int(128)
  )
  {
  return
  const int(1)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(target_char_ulong_conversion)
  (
  void
  )
  {
  char
  identifier(c)
  =
  const unsigned long(9223373136366403583ul)
  ;
  signed
  char
  identifier(c2)
  =
  const unsigned long(9223372036854775935ul)
  ;
  unsigned
  long
  identifier(ul)
  =
  (
  unsigned
  long
  )
  identifier(c)
  ;
  if
  (
  identifier(ul)
  !=
  const unsigned long(18446744073709551615UL)
  )
  {
  return
  const int(1)
  ;
  }
  identifier(ul)
  =
  (
  unsigned
  long
  )
  identifier(c2)
  ;
  if
  (
  identifier(ul)
  !=
  const unsigned long(127ul)
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
  identifier(target_uchar_ulong_conversion)
  (
  void
  )
  {
  unsigned
  char
  identifier(uc)
  =
  const unsigned long(9223372037929566207ul)
  ;
  unsigned
  int
  identifier(ui)
  =
  (
  unsigned
  int
  )
  identifier(uc)
  ;
  if
  (
  identifier(ui)
  !=
  const unsigned int(255u)
  )
  {
  return
  const int(1)
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
  identifier(target_extend_int_to_long)
  (
  )
  !=
  -
  const long(24019198012642978l)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(target_extend_int_to_ulong)
  (
  )
  !=
  const unsigned long(16ul)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_extend_uint_to_long)
  (
  )
  !=
  const long(2l)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_extend_uint_to_ulong)
  (
  )
  !=
  const unsigned long(1ul)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(target_truncate_long_to_int)
  (
  )
  !=
  -
  const int(20)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(target_truncate_long_to_uint)
  (
  )
  !=
  const unsigned int(214748364u)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(target_truncate_ulong_to_int)
  (
  )
  !=
  -
  const int(10)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(target_truncate_ulong_to_uint)
  (
  )
  !=
  const unsigned int(214748364u)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(target_char_int_conversion)
  (
  )
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(target_uchar_int_conversion)
  (
  )
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  identifier(target_char_uint_conversion)
  (
  )
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  identifier(target_uchar_uint_conversion)
  (
  )
  )
  {
  return
  const int(12)
  ;
  }
  if
  (
  identifier(target_char_long_conversion)
  (
  )
  )
  {
  return
  const int(13)
  ;
  }
  if
  (
  identifier(target_uchar_long_conversion)
  (
  )
  )
  {
  return
  const int(14)
  ;
  }
  if
  (
  identifier(target_char_ulong_conversion)
  (
  )
  )
  {
  return
  const int(15)
  ;
  }
  if
  (
  identifier(target_uchar_ulong_conversion)
  (
  )
  )
  {
  return
  const int(16)
  ;
  }
  return
  const int(0)
  ;
  }
