-- Lexing ... OK
+
+
@@ Tokens @@
List[498]:
  int
  identifier(long_to_int)
  (
  void
  )
  {
  return
  (
  int
  )
  const long(35218731828434l)
  ;
  }
  unsigned
  int
  identifier(long_to_uint)
  (
  void
  )
  {
  return
  (
  unsigned
  int
  )
  const long(35218731828434l)
  ;
  }
  char
  identifier(long_to_char)
  (
  void
  )
  {
  return
  (
  char
  )
  const long(9223372036854775807l)
  ;
  }
  signed
  char
  identifier(long_to_schar)
  (
  void
  )
  {
  return
  (
  signed
  char
  )
  const long(4611686018427388032l)
  ;
  }
  unsigned
  char
  identifier(long_to_uchar)
  (
  void
  )
  {
  return
  (
  unsigned
  char
  )
  const unsigned long(4294967295UL)
  ;
  }
  int
  identifier(ulong_to_int)
  (
  void
  )
  {
  return
  (
  int
  )
  const unsigned long(18446744073709551615UL)
  ;
  }
  unsigned
  int
  identifier(ulong_to_uint)
  (
  void
  )
  {
  return
  (
  unsigned
  int
  )
  const unsigned long(18446744073709551615UL)
  ;
  }
  char
  identifier(ulong_to_char)
  (
  void
  )
  {
  return
  (
  char
  )
  const unsigned long(4294967295UL)
  ;
  }
  signed
  char
  identifier(ulong_to_schar)
  (
  void
  )
  {
  return
  (
  signed
  char
  )
  const unsigned long(4611686018427388032ul)
  ;
  }
  unsigned
  char
  identifier(ulong_to_uchar)
  (
  void
  )
  {
  return
  (
  unsigned
  char
  )
  const unsigned long(9223372036854776063ul)
  ;
  }
  char
  identifier(int_to_char)
  (
  void
  )
  {
  return
  (
  char
  )
  const int(1274)
  ;
  }
  signed
  char
  identifier(int_to_schar)
  (
  void
  )
  {
  return
  (
  signed
  char
  )
  const int(2147483647)
  ;
  }
  unsigned
  char
  identifier(int_to_uchar)
  (
  void
  )
  {
  return
  (
  unsigned
  char
  )
  const int(1274)
  ;
  }
  char
  identifier(uint_to_char)
  (
  void
  )
  {
  return
  (
  char
  )
  const unsigned int(2147483901u)
  ;
  }
  signed
  char
  identifier(uint_to_schar)
  (
  void
  )
  {
  return
  (
  signed
  char
  )
  const unsigned int(2147483660u)
  ;
  }
  unsigned
  char
  identifier(uint_to_uchar)
  (
  void
  )
  {
  return
  (
  unsigned
  char
  )
  const unsigned int(2147483901u)
  ;
  }
  unsigned
  char
  identifier(implicit)
  (
  void
  )
  {
  return
  const unsigned int(2147483901u)
  ;
  }
  int
  identifier(one)
  =
  const int(1)
  ;
  int
  identifier(six)
  =
  const int(6)
  ;
  int
  identifier(three)
  =
  const int(3)
  ;
  int
  identifier(one_twenty_eight)
  =
  const int(128)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(long_to_int)
  (
  )
  !=
  const int(1234)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(long_to_uint)
  (
  )
  !=
  const unsigned int(1234u)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(long_to_char)
  (
  )
  !=
  -
  identifier(one)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(long_to_schar)
  (
  )
  !=
  -
  identifier(one_twenty_eight)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(long_to_uchar)
  (
  )
  !=
  const int(255)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(ulong_to_int)
  (
  )
  !=
  -
  identifier(one)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(ulong_to_uint)
  (
  )
  !=
  const unsigned int(4294967295U)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(ulong_to_char)
  (
  )
  !=
  -
  identifier(one)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(ulong_to_schar)
  (
  )
  !=
  -
  identifier(one_twenty_eight)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(ulong_to_uchar)
  (
  )
  !=
  const int(255)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  identifier(int_to_char)
  (
  )
  !=
  -
  identifier(six)
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  identifier(int_to_schar)
  (
  )
  !=
  -
  identifier(one)
  )
  {
  return
  const int(12)
  ;
  }
  if
  (
  identifier(int_to_uchar)
  (
  )
  !=
  const int(250)
  )
  {
  return
  const int(13)
  ;
  }
  if
  (
  identifier(uint_to_char)
  (
  )
  !=
  -
  identifier(three)
  )
  {
  return
  const int(14)
  ;
  }
  if
  (
  identifier(uint_to_schar)
  (
  )
  !=
  const int(12)
  )
  {
  return
  const int(15)
  ;
  }
  if
  (
  identifier(uint_to_uchar)
  (
  )
  !=
  const int(253)
  )
  {
  return
  const int(16)
  ;
  }
  if
  (
  identifier(implicit)
  (
  )
  !=
  const int(253)
  )
  {
  return
  const int(17)
  ;
  }
  return
  const int(0)
  ;
  }
