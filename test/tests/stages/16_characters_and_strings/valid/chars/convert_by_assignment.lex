-- Lexing ... OK
+
+
@@ Tokens @@
List[717]:
  int
  identifier(check_int)
  (
  int
  identifier(converted)
  ,
  int
  identifier(expected)
  )
  {
  return
  (
  identifier(converted)
  ==
  identifier(expected)
  )
  ;
  }
  int
  identifier(check_uint)
  (
  unsigned
  int
  identifier(converted)
  ,
  unsigned
  int
  identifier(expected)
  )
  {
  return
  (
  identifier(converted)
  ==
  identifier(expected)
  )
  ;
  }
  int
  identifier(check_long)
  (
  long
  identifier(converted)
  ,
  long
  identifier(expected)
  )
  {
  return
  (
  identifier(converted)
  ==
  identifier(expected)
  )
  ;
  }
  int
  identifier(check_ulong)
  (
  unsigned
  long
  identifier(converted)
  ,
  unsigned
  long
  identifier(expected)
  )
  {
  return
  (
  identifier(converted)
  ==
  identifier(expected)
  )
  ;
  }
  int
  identifier(check_double)
  (
  double
  identifier(converted)
  ,
  double
  identifier(expected)
  )
  {
  return
  (
  identifier(converted)
  ==
  identifier(expected)
  )
  ;
  }
  int
  identifier(check_char)
  (
  char
  identifier(converted)
  ,
  char
  identifier(expected)
  )
  {
  return
  (
  identifier(converted)
  ==
  identifier(expected)
  )
  ;
  }
  int
  identifier(check_uchar)
  (
  unsigned
  char
  identifier(converted)
  ,
  unsigned
  char
  identifier(expected)
  )
  {
  return
  (
  identifier(converted)
  ==
  identifier(expected)
  )
  ;
  }
  int
  identifier(check_char_on_stack)
  (
  signed
  char
  identifier(expected)
  ,
  int
  identifier(dummy1)
  ,
  int
  identifier(dummy2)
  ,
  int
  identifier(dummy3)
  ,
  int
  identifier(dummy4)
  ,
  int
  identifier(dummy5)
  ,
  int
  identifier(dummy6)
  ,
  signed
  char
  identifier(converted)
  )
  {
  return
  identifier(converted)
  ==
  identifier(expected)
  ;
  }
  int
  identifier(return_extended_uchar)
  (
  unsigned
  char
  identifier(c)
  )
  {
  return
  identifier(c)
  ;
  }
  unsigned
  long
  identifier(return_extended_schar)
  (
  signed
  char
  identifier(sc)
  )
  {
  return
  identifier(sc)
  ;
  }
  unsigned
  char
  identifier(return_truncated_long)
  (
  long
  identifier(l)
  )
  {
  return
  identifier(l)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  signed
  char
  identifier(sc)
  =
  -
  const int(10)
  ;
  if
  (
  !
  identifier(check_long)
  (
  identifier(sc)
  ,
  -
  const long(10l)
  )
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  !
  identifier(check_uint)
  (
  identifier(sc)
  ,
  const unsigned int(4294967286u)
  )
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  !
  identifier(check_double)
  (
  identifier(sc)
  ,
  -
  const double(10.0)
  )
  )
  {
  return
  const int(3)
  ;
  }
  unsigned
  char
  identifier(uc)
  =
  const int(246)
  ;
  if
  (
  !
  identifier(check_uchar)
  (
  identifier(sc)
  ,
  identifier(uc)
  )
  )
  {
  return
  const int(4)
  ;
  }
  char
  identifier(c)
  =
  -
  const int(10)
  ;
  if
  (
  !
  identifier(check_char)
  (
  -
  const int(10)
  ,
  identifier(c)
  )
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  !
  identifier(check_char)
  (
  const unsigned int(4294967286u)
  ,
  identifier(c)
  )
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  !
  identifier(check_char)
  (
  -
  const double(10.0)
  ,
  identifier(c)
  )
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  !
  identifier(check_char_on_stack)
  (
  identifier(c)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  -
  const double(10.0)
  )
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  !
  identifier(check_int)
  (
  identifier(uc)
  ,
  const int(246)
  )
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  !
  identifier(check_ulong)
  (
  identifier(uc)
  ,
  const unsigned long(246ul)
  )
  )
  {
  return
  const int(10)
  ;
  }
  char
  identifier(expected_char)
  =
  -
  const int(10)
  ;
  if
  (
  !
  identifier(check_char)
  (
  identifier(uc)
  ,
  identifier(expected_char)
  )
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  !
  identifier(check_uchar)
  (
  const unsigned long(18446744073709551606ul)
  ,
  identifier(uc)
  )
  )
  {
  return
  const int(12)
  ;
  }
  if
  (
  identifier(return_extended_uchar)
  (
  identifier(uc)
  )
  !=
  const int(246)
  )
  {
  return
  const int(13)
  ;
  }
  if
  (
  identifier(return_extended_schar)
  (
  identifier(sc)
  )
  !=
  const unsigned long(18446744073709551606ul)
  )
  {
  return
  const int(14)
  ;
  }
  if
  (
  identifier(return_truncated_long)
  (
  const long(5369233654l)
  )
  !=
  identifier(uc)
  )
  {
  return
  const int(15)
  ;
  }
  char
  identifier(array)
  [
  const int(3)
  ]
  =
  {
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  }
  ;
  identifier(array)
  [
  const int(1)
  ]
  =
  const int(128)
  ;
  if
  (
  identifier(array)
  [
  const int(0)
  ]
  ||
  identifier(array)
  [
  const int(2)
  ]
  ||
  identifier(array)
  [
  const int(1)
  ]
  !=
  -
  const int(128)
  )
  {
  return
  const int(16)
  ;
  }
  identifier(array)
  [
  const int(1)
  ]
  =
  const unsigned long(9224497936761618562ul)
  ;
  if
  (
  identifier(array)
  [
  const int(0)
  ]
  ||
  identifier(array)
  [
  const int(2)
  ]
  ||
  identifier(array)
  [
  const int(1)
  ]
  !=
  -
  const int(126)
  )
  {
  return
  const int(17)
  ;
  }
  identifier(array)
  [
  const int(1)
  ]
  =
  -
  const double(2.6)
  ;
  if
  (
  identifier(array)
  [
  const int(0)
  ]
  ||
  identifier(array)
  [
  const int(2)
  ]
  ||
  identifier(array)
  [
  const int(1)
  ]
  !=
  -
  const int(2)
  )
  {
  return
  const int(18)
  ;
  }
  unsigned
  char
  identifier(uchar_array)
  [
  const int(3)
  ]
  =
  {
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  }
  ;
  identifier(uchar_array)
  [
  const int(1)
  ]
  =
  const long(17592186044416l)
  ;
  if
  (
  identifier(uchar_array)
  [
  const int(0)
  ]
  ||
  identifier(uchar_array)
  [
  const int(2)
  ]
  ||
  identifier(uchar_array)
  [
  const int(1)
  ]
  !=
  const int(0)
  )
  {
  return
  const int(19)
  ;
  }
  identifier(uchar_array)
  [
  const int(1)
  ]
  =
  const unsigned int(2147483898u)
  ;
  if
  (
  identifier(uchar_array)
  [
  const int(0)
  ]
  ||
  identifier(uchar_array)
  [
  const int(2)
  ]
  ||
  identifier(uchar_array)
  [
  const int(1)
  ]
  !=
  const int(250)
  )
  {
  return
  const int(20)
  ;
  }
  unsigned
  int
  identifier(ui)
  =
  const unsigned int(4294967295U)
  ;
  static
  unsigned
  char
  identifier(uc_static)
  ;
  identifier(ui)
  =
  identifier(uc_static)
  ;
  if
  (
  identifier(ui)
  )
  {
  return
  const int(21)
  ;
  }
  signed
  long
  identifier(l)
  =
  -
  const int(1)
  ;
  static
  signed
  identifier(s_static)
  =
  const int(0)
  ;
  identifier(l)
  =
  identifier(s_static)
  ;
  if
  (
  identifier(l)
  )
  {
  return
  const int(22)
  ;
  }
  return
  const int(0)
  ;
  }
