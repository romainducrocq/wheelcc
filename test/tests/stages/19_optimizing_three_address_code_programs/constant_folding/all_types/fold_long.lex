-- Lexing ... OK
+
+
@@ Tokens @@
List[368]:
  long
  identifier(target_add)
  (
  void
  )
  {
  return
  const long(2147483647l)
  +
  const long(1000l)
  ;
  }
  long
  identifier(target_sub)
  (
  void
  )
  {
  return
  const long(1000l)
  -
  const long(9223372036854773807l)
  ;
  }
  long
  identifier(target_mult)
  (
  void
  )
  {
  return
  const long(35184372088832l)
  *
  const long(4l)
  ;
  }
  long
  identifier(target_div)
  (
  void
  )
  {
  return
  const long(9223372036854775807l)
  /
  const long(3147483647l)
  ;
  }
  long
  identifier(target_rem)
  (
  void
  )
  {
  return
  const long(9223372036854775807l)
  %
  const long(3147483647l)
  ;
  }
  long
  identifier(target_complement)
  (
  void
  )
  {
  return
  ~
  const long(6148914691236517206l)
  ;
  }
  long
  identifier(target_neg)
  (
  void
  )
  {
  return
  -
  (
  const long(9223372036854775716l)
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
  const long(72110370596061184l)
  ;
  }
  int
  identifier(target_eq)
  (
  void
  )
  {
  return
  const long(9223372036854775716l)
  ==
  const long(9223372036854775716l)
  ;
  }
  int
  identifier(target_neq)
  (
  void
  )
  {
  return
  const long(72110370596061184l)
  !=
  const long(0l)
  ;
  }
  int
  identifier(target_gt)
  (
  void
  )
  {
  return
  const long(549755813889l)
  >
  const long(17592186044416l)
  ;
  }
  int
  identifier(target_ge)
  (
  void
  )
  {
  return
  const long(400l)
  >=
  const long(399l)
  ;
  }
  int
  identifier(target_lt)
  (
  void
  )
  {
  return
  const long(17592186044416l)
  <
  const long(549755813888l)
  ;
  }
  int
  identifier(target_le)
  (
  void
  )
  {
  return
  const long(2147483648l)
  <=
  const long(0l)
  ;
  }
  long
  identifier(sub_result)
  =
  const long(9223372036854772807l)
  ;
  long
  identifier(complement_result)
  =
  const long(6148914691236517207l)
  ;
  long
  identifier(neg_result)
  =
  const long(9223372036854775716l)
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
  const long(2147484647l)
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
  identifier(sub_result)
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
  const long(140737488355328l)
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
  const long(2930395538l)
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
  const long(1758008721l)
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
  -
  identifier(complement_result)
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
  !=
  -
  identifier(neg_result)
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
