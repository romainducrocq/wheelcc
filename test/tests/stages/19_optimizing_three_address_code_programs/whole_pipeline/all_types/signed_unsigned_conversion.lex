-- Lexing ... OK
+
+
@@ Tokens @@
List[182]:
  unsigned
  int
  identifier(target_int_to_uint)
  (
  void
  )
  {
  int
  identifier(i)
  =
  -
  const int(1)
  ;
  unsigned
  int
  identifier(u)
  =
  (
  unsigned
  )
  identifier(i)
  ;
  return
  identifier(u)
  /
  const unsigned int(10u)
  ;
  }
  int
  identifier(target_uint_to_int)
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
  int
  identifier(i)
  =
  (
  int
  )
  identifier(u)
  ;
  return
  (
  identifier(i)
  +
  const int(1)
  )
  ?
  const int(0)
  :
  identifier(i)
  *
  const int(2)
  ;
  }
  long
  identifier(target_ulong_to_long)
  (
  void
  )
  {
  unsigned
  long
  identifier(ul)
  =
  const unsigned long(9223372036854775900ul)
  ;
  signed
  long
  identifier(l)
  =
  (
  long
  )
  identifier(ul)
  ;
  return
  identifier(l)
  /
  const int(4)
  ;
  }
  unsigned
  long
  identifier(target_long_to_ulong)
  (
  void
  )
  {
  long
  identifier(l)
  =
  -
  const long(200l)
  ;
  unsigned
  long
  identifier(ul)
  =
  (
  unsigned
  long
  )
  identifier(l)
  ;
  return
  identifier(ul)
  /
  const int(10)
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
  identifier(target_int_to_uint)
  (
  )
  !=
  const unsigned int(429496729u)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(target_uint_to_int)
  (
  )
  !=
  -
  const int(2)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_ulong_to_long)
  (
  )
  !=
  -
  const int(2305843009213693929)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_long_to_ulong)
  (
  )
  !=
  const unsigned long(1844674407370955141ul)
  )
  {
  return
  const int(4)
  ;
  }
  return
  const int(0)
  ;
  }
