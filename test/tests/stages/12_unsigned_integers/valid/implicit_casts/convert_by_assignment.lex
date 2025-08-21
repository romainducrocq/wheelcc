-- Lexing ... OK
+
+
@@ Tokens @@
List[251]:
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
  long
  identifier(return_extended_uint)
  (
  unsigned
  int
  identifier(u)
  )
  {
  return
  identifier(u)
  ;
  }
  unsigned
  long
  identifier(return_extended_int)
  (
  int
  identifier(i)
  )
  {
  return
  identifier(i)
  ;
  }
  int
  identifier(return_truncated_ulong)
  (
  unsigned
  long
  identifier(ul)
  )
  {
  return
  identifier(ul)
  ;
  }
  int
  identifier(extend_on_assignment)
  (
  unsigned
  int
  identifier(ui)
  ,
  long
  identifier(expected)
  )
  {
  long
  identifier(result)
  =
  identifier(ui)
  ;
  return
  identifier(result)
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
  identifier(check_int)
  (
  const unsigned long(9223372036854775813ul)
  ,
  const int(5)
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
  identifier(check_long)
  (
  const unsigned int(2147483658u)
  ,
  const long(2147483658l)
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
  identifier(check_ulong)
  (
  -
  const int(1)
  ,
  const unsigned long(18446744073709551615UL)
  )
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(return_extended_uint)
  (
  const unsigned int(2147483658u)
  )
  !=
  const long(2147483658l)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(return_extended_int)
  (
  -
  const int(1)
  )
  !=
  const unsigned long(18446744073709551615UL)
  )
  {
  return
  const int(5)
  ;
  }
  long
  identifier(l)
  =
  identifier(return_truncated_ulong)
  (
  const unsigned long(1125902054326372ul)
  )
  ;
  if
  (
  identifier(l)
  !=
  -
  const long(2147483548l)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  !
  identifier(extend_on_assignment)
  (
  const unsigned int(2147483658u)
  ,
  const long(2147483658l)
  )
  )
  {
  return
  const int(7)
  ;
  }
  int
  identifier(i)
  =
  const unsigned int(4294967196u)
  ;
  if
  (
  identifier(i)
  !=
  -
  const int(100)
  )
  {
  return
  const int(8)
  ;
  }
  return
  const int(0)
  ;
  }
