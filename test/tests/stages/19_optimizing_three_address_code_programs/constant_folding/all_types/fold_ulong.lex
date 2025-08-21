-- Lexing ... OK
+
+
@@ Tokens @@
List[384]:
  unsigned
  long
  identifier(target_add)
  (
  void
  )
  {
  return
  const unsigned long(18446744073709551615UL)
  +
  const unsigned long(10ul)
  ;
  }
  unsigned
  long
  identifier(target_sub)
  (
  void
  )
  {
  return
  const unsigned long(10ul)
  -
  const unsigned long(12ul)
  ;
  }
  unsigned
  long
  identifier(target_mult)
  (
  void
  )
  {
  return
  const unsigned long(9223372036854775808ul)
  *
  const unsigned long(3ul)
  ;
  }
  unsigned
  long
  identifier(target_div)
  (
  void
  )
  {
  return
  const unsigned long(18446744073709551614ul)
  /
  const unsigned long(10ul)
  ;
  }
  unsigned
  long
  identifier(target_rem)
  (
  void
  )
  {
  return
  const unsigned long(18446744073709551614ul)
  %
  const unsigned long(10ul)
  ;
  }
  unsigned
  long
  identifier(target_complement)
  (
  void
  )
  {
  return
  ~
  const unsigned long(1ul)
  ;
  }
  unsigned
  long
  identifier(target_neg)
  (
  void
  )
  {
  return
  -
  (
  const unsigned long(9223372036854775900ul)
  )
  ;
  }
  int
  identifier(target_not)
  (
  void
  )
  {
  return
  !
  const unsigned long(4294967296UL)
  ;
  }
  int
  identifier(target_eq)
  (
  void
  )
  {
  return
  const unsigned long(18446744073709551615UL)
  ==
  const unsigned long(18446744073709551615UL)
  ;
  }
  int
  identifier(target_neq)
  (
  void
  )
  {
  return
  const unsigned long(9223372036854775809ul)
  !=
  const unsigned long(1ul)
  ;
  }
  int
  identifier(target_gt)
  (
  void
  )
  {
  return
  const unsigned long(9223372036854775809ul)
  >
  const unsigned long(1000ul)
  ;
  }
  int
  identifier(target_ge)
  (
  void
  )
  {
  return
  const unsigned long(9223372036854775809ul)
  >=
  const unsigned long(200ul)
  ;
  }
  int
  identifier(target_lt)
  (
  void
  )
  {
  return
  const unsigned long(9223372036854775809ul)
  <
  const unsigned long(1000ul)
  ;
  }
  int
  identifier(target_le)
  (
  void
  )
  {
  return
  const unsigned long(9223372036854775809ul)
  <=
  const unsigned long(200ul)
  ;
  }
  int
  identifier(target_le2)
  (
  void
  )
  {
  return
  const unsigned long(9223372036854775809ul)
  <=
  const unsigned long(9223372036854775809ul)
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
  identifier(target_add)
  (
  )
  !=
  const unsigned long(9ul)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(target_sub)
  (
  )
  !=
  const unsigned long(18446744073709551614ul)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_mult)
  (
  )
  !=
  const unsigned long(9223372036854775808ul)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_div)
  (
  )
  !=
  const unsigned long(1844674407370955161ul)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(target_rem)
  (
  )
  !=
  const unsigned long(4ul)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(target_complement)
  (
  )
  !=
  const unsigned long(18446744073709551614ul)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(target_neg)
  (
  )
  +
  const unsigned long(9223372036854775900ul)
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
  identifier(target_not)
  (
  )
  !=
  const int(0)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  !
  identifier(target_eq)
  (
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
  identifier(target_neq)
  (
  )
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  !
  identifier(target_gt)
  (
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
  identifier(target_ge)
  (
  )
  )
  {
  return
  const int(12)
  ;
  }
  if
  (
  identifier(target_lt)
  (
  )
  )
  {
  return
  const int(13)
  ;
  }
  if
  (
  identifier(target_le)
  (
  )
  )
  {
  return
  const int(14)
  ;
  }
  if
  (
  !
  identifier(target_le2)
  (
  )
  )
  {
  return
  const int(15)
  ;
  }
  return
  const int(0)
  ;
  }
