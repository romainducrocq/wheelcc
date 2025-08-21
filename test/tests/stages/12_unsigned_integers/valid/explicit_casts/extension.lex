-- Lexing ... OK
+
+
@@ Tokens @@
List[159]:
  int
  identifier(int_to_ulong)
  (
  int
  identifier(i)
  ,
  unsigned
  long
  identifier(expected)
  )
  {
  unsigned
  long
  identifier(result)
  =
  (
  unsigned
  long
  )
  identifier(i)
  ;
  return
  identifier(result)
  ==
  identifier(expected)
  ;
  }
  int
  identifier(uint_to_long)
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
  (
  long
  )
  identifier(ui)
  ;
  return
  identifier(result)
  ==
  identifier(expected)
  ;
  }
  int
  identifier(uint_to_ulong)
  (
  unsigned
  identifier(ui)
  ,
  unsigned
  long
  identifier(expected)
  )
  {
  return
  (
  unsigned
  long
  )
  identifier(ui)
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
  identifier(int_to_ulong)
  (
  const int(10)
  ,
  const unsigned long(10ul)
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
  identifier(int_to_ulong)
  (
  -
  const int(10)
  ,
  const unsigned long(18446744073709551606ul)
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
  identifier(uint_to_long)
  (
  const unsigned int(4294967200u)
  ,
  const long(4294967200l)
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
  identifier(uint_to_ulong)
  (
  const unsigned int(4294967200u)
  ,
  const unsigned long(4294967200ul)
  )
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  (
  unsigned
  long
  )
  const unsigned int(4294967200u)
  !=
  const unsigned long(4294967200ul)
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
