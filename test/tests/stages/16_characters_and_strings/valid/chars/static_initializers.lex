-- Lexing ... OK
+
+
@@ Tokens @@
List[229]:
  char
  identifier(from_long)
  =
  const long(17592186044416l)
  ;
  char
  identifier(from_double)
  =
  const double(15.6)
  ;
  char
  identifier(from_uint)
  =
  const unsigned int(2147483777u)
  ;
  char
  identifier(from_ulong)
  =
  const unsigned long(9223372037928517642ul)
  ;
  signed
  char
  identifier(schar_from_long)
  =
  const long(17592186044419l)
  ;
  signed
  char
  identifier(schar_from_uint)
  =
  const unsigned int(2147483898u)
  ;
  signed
  char
  identifier(schar_from_ulong)
  =
  const unsigned long(9223372037928517642ul)
  ;
  signed
  char
  identifier(schar_from_double)
  =
  const double(1e-10)
  ;
  unsigned
  char
  identifier(uchar_from_int)
  =
  const int(13526)
  ;
  unsigned
  char
  identifier(uchar_from_uint)
  =
  const unsigned int(2147483898u)
  ;
  unsigned
  char
  identifier(uchar_from_long)
  =
  const long(1101659111674l)
  ;
  unsigned
  char
  identifier(uchar_from_ulong)
  =
  const unsigned long(9223372037928517642ul)
  ;
  unsigned
  char
  identifier(uchar_from_double)
  =
  const double(77.7)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(from_long)
  !=
  const int(0)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(from_double)
  !=
  const int(15)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(from_uint)
  !=
  -
  const int(127)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(from_ulong)
  !=
  const int(10)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(schar_from_uint)
  !=
  -
  const int(6)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(schar_from_ulong)
  !=
  const int(10)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(schar_from_double)
  !=
  const int(0)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(uchar_from_int)
  !=
  const int(214)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(uchar_from_uint)
  !=
  const int(250)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(uchar_from_ulong)
  !=
  const int(10)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  identifier(uchar_from_double)
  !=
  const int(77)
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  identifier(schar_from_long)
  !=
  const int(3)
  )
  {
  return
  const int(12)
  ;
  }
  if
  (
  identifier(uchar_from_long)
  !=
  const int(250)
  )
  {
  return
  const int(13)
  ;
  }
  return
  const int(0)
  ;
  }
