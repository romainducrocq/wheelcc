-- Lexing ... OK
+
+
@@ Tokens @@
List[274]:
  double
  identifier(copysign)
  (
  double
  identifier(x)
  ,
  double
  identifier(y)
  )
  ;
  double
  identifier(target_from_neg_int)
  (
  void
  )
  {
  return
  (
  double
  )
  -
  const int(2147483647)
  ;
  }
  double
  identifier(target_from_neg_long)
  (
  void
  )
  {
  return
  (
  double
  )
  -
  const long(4611686018427388416l)
  ;
  }
  double
  identifier(target_from_char)
  (
  void
  )
  {
  char
  identifier(c)
  =
  const int(127)
  ;
  return
  (
  double
  )
  identifier(c)
  ;
  }
  double
  identifier(target_from_schar)
  (
  void
  )
  {
  char
  identifier(c)
  =
  -
  const int(127)
  ;
  return
  (
  double
  )
  identifier(c)
  ;
  }
  double
  identifier(target_from_uchar)
  (
  void
  )
  {
  unsigned
  char
  identifier(u)
  =
  const int(255)
  ;
  return
  (
  double
  )
  identifier(u)
  ;
  }
  double
  identifier(target_from_truncated_char)
  (
  void
  )
  {
  char
  identifier(c)
  =
  -
  const int(129)
  ;
  return
  (
  double
  )
  identifier(c)
  ;
  }
  double
  identifier(target_from_truncated_uchar)
  (
  void
  )
  {
  unsigned
  char
  identifier(c)
  =
  const int(1000)
  ;
  return
  (
  double
  )
  identifier(c)
  ;
  }
  double
  identifier(target_from_negated_int_zero)
  (
  void
  )
  {
  return
  -
  const int(0)
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
  identifier(target_from_neg_int)
  (
  )
  !=
  -
  const double(2147483647.)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(target_from_neg_long)
  (
  )
  !=
  -
  const double(4611686018427387904.0)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_from_char)
  (
  )
  !=
  const int(127)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_from_schar)
  (
  )
  !=
  -
  const int(127)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(target_from_uchar)
  (
  )
  !=
  const int(255)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(target_from_truncated_char)
  (
  )
  !=
  const int(127)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(target_from_truncated_uchar)
  (
  )
  !=
  const int(232)
  )
  {
  return
  const int(7)
  ;
  }
  double
  identifier(zero)
  =
  identifier(target_from_negated_int_zero)
  (
  )
  ;
  if
  (
  identifier(zero)
  !=
  const int(0)
  ||
  identifier(copysign)
  (
  const double(5.)
  ,
  identifier(zero)
  )
  !=
  const double(5.)
  )
  {
  return
  const int(8)
  ;
  }
  return
  const int(0)
  ;
  }
