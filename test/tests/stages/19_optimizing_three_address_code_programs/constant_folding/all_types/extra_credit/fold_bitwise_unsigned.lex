-- Lexing ... OK
+
+
@@ Tokens @@
List[191]:
  unsigned
  identifier(target_and)
  (
  void
  )
  {
  return
  const unsigned int(4042322160u)
  &
  const unsigned int(4278255360u)
  ;
  }
  unsigned
  long
  identifier(target_or)
  (
  void
  )
  {
  return
  const unsigned long(1085102592571150095ul)
  |
  const unsigned long(18374966859414961920ul)
  ;
  }
  unsigned
  int
  identifier(target_xor)
  (
  void
  )
  {
  return
  const unsigned int(4042322160u)
  ^
  const unsigned int(267390960u)
  ;
  }
  unsigned
  int
  identifier(target_shift_uint_left)
  (
  void
  )
  {
  return
  const unsigned int(10u)
  <<
  const long(24l)
  ;
  }
  unsigned
  long
  identifier(target_shift_ulong_left)
  (
  void
  )
  {
  return
  const unsigned long(2286249799ul)
  <<
  const unsigned int(33u)
  ;
  }
  unsigned
  int
  identifier(target_shift_uint_right)
  (
  void
  )
  {
  return
  const unsigned int(4294967296u)
  >>
  const int(16)
  ;
  }
  unsigned
  long
  identifier(target_shift_ulong_right)
  (
  void
  )
  {
  return
  const unsigned long(9223372041149743104ul)
  >>
  const long(21l)
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
  identifier(target_and)
  (
  )
  !=
  const unsigned int(4026593280u)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(target_or)
  (
  )
  !=
  const unsigned long(18379189048491114255ul)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_xor)
  (
  )
  !=
  const unsigned int(4278255360u)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_shift_uint_left)
  (
  )
  !=
  const unsigned int(167772160u)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(target_shift_ulong_left)
  (
  )
  !=
  const unsigned long(1191992160673595392ul)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(target_shift_uint_right)
  (
  )
  !=
  const unsigned int(65536u)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(target_shift_ulong_right)
  (
  )
  !=
  const unsigned long(4398046513152ul)
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
