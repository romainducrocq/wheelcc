-- Lexing ... OK
+
+
@@ Tokens @@
List[358]:
  unsigned
  int
  identifier(target_add)
  (
  void
  )
  {
  return
  const unsigned int(4294967295U)
  +
  const unsigned int(10u)
  ;
  }
  unsigned
  int
  identifier(target_sub)
  (
  void
  )
  {
  return
  const unsigned int(10u)
  -
  const unsigned int(12u)
  ;
  }
  unsigned
  int
  identifier(target_mult)
  (
  void
  )
  {
  return
  const unsigned int(2147483648u)
  *
  const unsigned int(3u)
  ;
  }
  unsigned
  int
  identifier(target_div)
  (
  void
  )
  {
  return
  const unsigned int(4294967286u)
  /
  const unsigned int(10u)
  ;
  }
  unsigned
  int
  identifier(target_rem)
  (
  void
  )
  {
  return
  const unsigned int(4294967286u)
  %
  const unsigned int(10u)
  ;
  }
  unsigned
  int
  identifier(target_complement)
  (
  void
  )
  {
  return
  ~
  const unsigned int(1u)
  ;
  }
  unsigned
  int
  identifier(target_neg)
  (
  void
  )
  {
  return
  -
  const unsigned int(10u)
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
  const unsigned int(65536u)
  ;
  }
  int
  identifier(target_eq)
  (
  void
  )
  {
  return
  const unsigned int(100u)
  ==
  const unsigned int(100u)
  ;
  }
  int
  identifier(target_neq)
  (
  void
  )
  {
  return
  const unsigned int(2147483649u)
  !=
  const unsigned int(1u)
  ;
  }
  int
  identifier(target_gt)
  (
  void
  )
  {
  return
  const unsigned int(2147483649u)
  >
  const unsigned int(1000u)
  ;
  }
  int
  identifier(target_ge)
  (
  void
  )
  {
  return
  const unsigned int(4000000000u)
  >=
  const unsigned int(3999999999u)
  ;
  }
  int
  identifier(target_lt)
  (
  void
  )
  {
  return
  const unsigned int(2147483649u)
  <
  const unsigned int(1000u)
  ;
  }
  int
  identifier(target_le)
  (
  void
  )
  {
  return
  const unsigned int(4000000000u)
  <=
  const unsigned int(3999999999u)
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
  const unsigned int(9u)
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
  const unsigned int(4294967294U)
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
  const unsigned int(2147483648u)
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
  const unsigned int(429496728u)
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
  const unsigned int(6u)
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
  const unsigned int(4294967294U)
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
  const int(10)
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
  return
  const int(0)
  ;
  }
