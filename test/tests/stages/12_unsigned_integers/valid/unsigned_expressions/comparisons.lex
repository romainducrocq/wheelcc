-- Lexing ... OK
+
+
@@ Tokens @@
List[202]:
  unsigned
  int
  identifier(one_hundred)
  =
  const unsigned int(100u)
  ;
  unsigned
  int
  identifier(large_uint)
  =
  const unsigned int(4294967294u)
  ;
  unsigned
  long
  identifier(one_hundred_ulong)
  =
  const unsigned long(100ul)
  ;
  unsigned
  long
  identifier(large_ulong)
  =
  const unsigned long(4294967294ul)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(large_uint)
  <
  identifier(one_hundred)
  )
  return
  const int(1)
  ;
  if
  (
  identifier(large_uint)
  <=
  identifier(one_hundred)
  )
  return
  const int(2)
  ;
  if
  (
  identifier(one_hundred)
  >=
  identifier(large_uint)
  )
  return
  const int(3)
  ;
  if
  (
  identifier(one_hundred)
  >
  identifier(large_uint)
  )
  return
  const int(4)
  ;
  if
  (
  !
  (
  identifier(one_hundred)
  <=
  identifier(large_uint)
  )
  )
  return
  const int(5)
  ;
  if
  (
  !
  (
  identifier(one_hundred)
  <
  identifier(large_uint)
  )
  )
  return
  const int(6)
  ;
  if
  (
  !
  (
  identifier(large_uint)
  >
  identifier(one_hundred)
  )
  )
  return
  const int(7)
  ;
  if
  (
  !
  (
  identifier(large_uint)
  >=
  identifier(one_hundred)
  )
  )
  return
  const int(8)
  ;
  if
  (
  identifier(large_ulong)
  <
  identifier(one_hundred_ulong)
  )
  return
  const int(9)
  ;
  if
  (
  identifier(large_ulong)
  <=
  identifier(one_hundred_ulong)
  )
  return
  const int(10)
  ;
  if
  (
  identifier(one_hundred_ulong)
  >=
  identifier(large_ulong)
  )
  return
  const int(11)
  ;
  if
  (
  identifier(one_hundred_ulong)
  >
  identifier(large_ulong)
  )
  return
  const int(12)
  ;
  if
  (
  !
  (
  identifier(one_hundred_ulong)
  <=
  identifier(large_ulong)
  )
  )
  return
  const int(13)
  ;
  if
  (
  !
  (
  identifier(one_hundred_ulong)
  <
  identifier(large_ulong)
  )
  )
  return
  const int(14)
  ;
  if
  (
  !
  (
  identifier(large_ulong)
  >
  identifier(one_hundred_ulong)
  )
  )
  return
  const int(15)
  ;
  if
  (
  !
  (
  identifier(large_ulong)
  >=
  identifier(one_hundred_ulong)
  )
  )
  return
  const int(16)
  ;
  return
  const int(0)
  ;
  }
