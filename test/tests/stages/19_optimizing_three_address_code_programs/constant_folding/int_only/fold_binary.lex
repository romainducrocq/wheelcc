-- Lexing ... OK
+
+
@@ Tokens @@
List[423]:
  int
  identifier(target_add)
  (
  void
  )
  {
  return
  const int(100)
  +
  const int(200)
  ;
  }
  int
  identifier(target_sub)
  (
  void
  )
  {
  return
  const int(2)
  -
  const int(2147483647)
  ;
  }
  int
  identifier(target_mult)
  (
  void
  )
  {
  return
  const int(1000)
  *
  const int(1000)
  ;
  }
  int
  identifier(target_div)
  (
  void
  )
  {
  return
  const int(1111)
  /
  const int(4)
  ;
  }
  int
  identifier(target_rem)
  (
  void
  )
  {
  return
  const int(10)
  %
  const int(3)
  ;
  }
  int
  identifier(target_eq_true)
  (
  void
  )
  {
  return
  const int(2147483647)
  ==
  const int(2147483647)
  ;
  }
  int
  identifier(target_eq_false)
  (
  void
  )
  {
  return
  const int(2147483647)
  ==
  const int(2147483646)
  ;
  }
  int
  identifier(target_neq_true)
  (
  void
  )
  {
  return
  const int(1111)
  !=
  const int(1112)
  ;
  }
  int
  identifier(target_neq_false)
  (
  void
  )
  {
  return
  const int(1112)
  !=
  const int(1112)
  ;
  }
  int
  identifier(target_gt_true)
  (
  void
  )
  {
  return
  const int(10)
  >
  const int(1)
  ;
  }
  int
  identifier(target_gt_false)
  (
  void
  )
  {
  return
  const int(10)
  >
  const int(10)
  ;
  }
  int
  identifier(target_ge_true)
  (
  void
  )
  {
  return
  const int(123456)
  >=
  const int(123456)
  ;
  }
  int
  identifier(target_ge_false)
  (
  void
  )
  {
  return
  const int(2147)
  >=
  const int(123456)
  ;
  }
  int
  identifier(target_lt_true)
  (
  void
  )
  {
  return
  const int(256)
  <
  const int(1073742080)
  ;
  }
  int
  identifier(target_lt_false)
  (
  void
  )
  {
  return
  const int(256)
  <
  const int(0)
  ;
  }
  int
  identifier(target_le_true)
  (
  void
  )
  {
  return
  const int(123456)
  <=
  const int(123457)
  ;
  }
  int
  identifier(target_le_false)
  (
  void
  )
  {
  return
  const int(123458)
  <=
  const int(123457)
  ;
  }
  int
  identifier(val_to_negate)
  =
  const int(2147483645)
  ;
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
  const int(300)
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
  -
  identifier(val_to_negate)
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
  const int(1000000)
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
  const int(277)
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
  const int(1)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(target_eq_false)
  (
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
  identifier(target_eq_true)
  (
  )
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(target_neq_false)
  (
  )
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  !
  identifier(target_neq_true)
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
  identifier(target_gt_false)
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
  identifier(target_gt_true)
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
  identifier(target_ge_false)
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
  !
  identifier(target_ge_true)
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
  identifier(target_lt_false)
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
  identifier(target_lt_true)
  (
  )
  )
  {
  return
  const int(15)
  ;
  }
  if
  (
  identifier(target_le_false)
  (
  )
  )
  {
  return
  const int(16)
  ;
  }
  if
  (
  !
  identifier(target_le_true)
  (
  )
  )
  {
  return
  const int(17)
  ;
  }
  return
  const int(0)
  ;
  }
