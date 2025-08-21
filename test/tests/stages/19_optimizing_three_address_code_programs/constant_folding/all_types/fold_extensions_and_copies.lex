-- Lexing ... OK
+
+
@@ Tokens @@
List[257]:
  long
  identifier(uint_to_long)
  (
  void
  )
  {
  return
  (
  long
  )
  const unsigned int(4294967295U)
  ;
  }
  unsigned
  long
  identifier(uint_to_ulong)
  (
  void
  )
  {
  return
  (
  unsigned
  long
  )
  const unsigned int(4294967295U)
  ;
  }
  unsigned
  long
  identifier(target_int_to_ulong)
  (
  void
  )
  {
  return
  (
  unsigned
  long
  )
  const int(2147483647)
  ;
  }
  long
  identifier(target_int_to_long)
  (
  void
  )
  {
  return
  (
  long
  )
  const int(1)
  ;
  }
  int
  identifier(uint_to_int)
  (
  void
  )
  {
  return
  (
  int
  )
  const unsigned int(4294967200U)
  ;
  }
  unsigned
  int
  identifier(int_to_uint)
  (
  void
  )
  {
  return
  (
  unsigned
  )
  const int(2147480000)
  ;
  }
  long
  identifier(ulong_to_long)
  (
  void
  )
  {
  return
  (
  long
  )
  const unsigned long(18446744073709551615UL)
  ;
  }
  unsigned
  long
  identifier(long_to_ulong)
  (
  void
  )
  {
  return
  const long(2147483650l)
  ;
  }
  long
  identifier(implicit)
  (
  void
  )
  {
  return
  const unsigned long(18446744073709551615UL)
  ;
  }
  long
  identifier(one)
  =
  const long(1l)
  ;
  long
  identifier(ninety_six)
  =
  const long(96l)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(uint_to_long)
  (
  )
  !=
  const long(4294967295l)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(uint_to_ulong)
  (
  )
  !=
  const unsigned long(4294967295ul)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_int_to_ulong)
  (
  )
  !=
  const long(2147483647l)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_int_to_long)
  (
  )
  !=
  const long(1l)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(uint_to_int)
  (
  )
  !=
  -
  identifier(ninety_six)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(int_to_uint)
  (
  )
  !=
  const unsigned int(2147480000u)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(ulong_to_long)
  (
  )
  !=
  -
  identifier(one)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(long_to_ulong)
  (
  )
  !=
  const unsigned long(2147483650ul)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(implicit)
  (
  )
  !=
  -
  identifier(one)
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
