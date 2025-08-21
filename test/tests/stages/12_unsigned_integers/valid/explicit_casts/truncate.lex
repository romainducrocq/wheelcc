-- Lexing ... OK
+
+
@@ Tokens @@
List[223]:
  int
  identifier(ulong_to_int)
  (
  unsigned
  long
  identifier(ul)
  ,
  int
  identifier(expected)
  )
  {
  int
  identifier(result)
  =
  (
  int
  )
  identifier(ul)
  ;
  return
  (
  identifier(result)
  ==
  identifier(expected)
  )
  ;
  }
  int
  identifier(ulong_to_uint)
  (
  unsigned
  long
  identifier(ul)
  ,
  unsigned
  identifier(expected)
  )
  {
  return
  (
  (
  unsigned
  int
  )
  identifier(ul)
  ==
  identifier(expected)
  )
  ;
  }
  int
  identifier(long_to_uint)
  (
  long
  identifier(l)
  ,
  unsigned
  int
  identifier(expected)
  )
  {
  return
  (
  unsigned
  int
  )
  identifier(l)
  ==
  identifier(expected)
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
  !
  identifier(long_to_uint)
  (
  const long(100l)
  ,
  const unsigned int(100u)
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
  identifier(long_to_uint)
  (
  -
  const long(9223372036854774574l)
  ,
  const unsigned int(1234u)
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
  identifier(ulong_to_int)
  (
  const unsigned long(100ul)
  ,
  const int(100)
  )
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  !
  identifier(ulong_to_uint)
  (
  const unsigned long(100ul)
  ,
  const unsigned int(100u)
  )
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  !
  identifier(ulong_to_uint)
  (
  const unsigned long(4294967200ul)
  ,
  const unsigned int(4294967200u)
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
  identifier(ulong_to_int)
  (
  const unsigned long(4294967200ul)
  ,
  -
  const int(96)
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
  identifier(ulong_to_uint)
  (
  const unsigned long(1152921506754330624ul)
  ,
  const unsigned int(2147483648u)
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
  identifier(ulong_to_int)
  (
  const unsigned long(1152921506754330624ul)
  ,
  -
  const int(2147483648)
  )
  )
  {
  return
  const int(8)
  ;
  }
  unsigned
  int
  identifier(ui)
  =
  (
  unsigned
  int
  )
  const unsigned long(17179869189ul)
  ;
  if
  (
  identifier(ui)
  !=
  const int(5)
  )
  return
  const int(9)
  ;
  return
  const int(0)
  ;
  }
