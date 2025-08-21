-- Lexing ... OK
+
+
@@ Tokens @@
List[194]:
  char
  identifier(target_to_char)
  (
  void
  )
  {
  return
  (
  char
  )
  const double(126.5)
  ;
  }
  unsigned
  char
  identifier(target_to_uchar)
  (
  void
  )
  {
  return
  (
  unsigned
  char
  )
  const double(254.9)
  ;
  }
  int
  identifier(target_to_int)
  (
  void
  )
  {
  return
  (
  int
  )
  const double(5.9)
  ;
  }
  unsigned
  identifier(target_to_uint)
  (
  void
  )
  {
  return
  (
  unsigned
  )
  const double(2147483750.5)
  ;
  }
  long
  identifier(target_to_long)
  (
  void
  )
  {
  return
  (
  long
  )
  const double(9223372036854774783.1)
  ;
  }
  unsigned
  long
  identifier(target_to_ulong)
  (
  void
  )
  {
  return
  (
  unsigned
  long
  )
  const double(13835058055282163712.5)
  ;
  }
  unsigned
  long
  identifier(target_implicit)
  (
  void
  )
  {
  return
  const double(3458764513821589504.0)
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
  identifier(target_to_char)
  (
  )
  !=
  const int(126)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(target_to_uchar)
  (
  )
  !=
  const int(254)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_to_int)
  (
  )
  !=
  const int(5)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_to_uint)
  (
  )
  !=
  const unsigned int(2147483750u)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(target_to_long)
  (
  )
  !=
  const long(9223372036854774784l)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(target_to_ulong)
  (
  )
  !=
  const unsigned long(13835058055282163712ul)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(target_implicit)
  (
  )
  !=
  const unsigned long(3458764513821589504ul)
  )
  {
  return
  const int(7)
  ;
  }
  return
  const int(0)
  ;
  }
