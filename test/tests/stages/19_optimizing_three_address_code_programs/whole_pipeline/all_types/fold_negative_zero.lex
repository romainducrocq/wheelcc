-- Lexing ... OK
+
+
@@ Tokens @@
List[1062]:
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
  int
  identifier(is_positive_zero)
  (
  double
  identifier(d)
  )
  {
  if
  (
  identifier(d)
  !=
  const double(0.0)
  )
  {
  return
  const int(0)
  ;
  }
  return
  (
  identifier(copysign)
  (
  const double(5.)
  ,
  identifier(d)
  )
  ==
  const double(5.0)
  )
  ;
  }
  int
  identifier(is_negative_zero)
  (
  double
  identifier(d)
  )
  {
  if
  (
  identifier(d)
  !=
  const double(0.0)
  )
  {
  return
  const int(0)
  ;
  }
  return
  (
  identifier(copysign)
  (
  const double(5.)
  ,
  identifier(d)
  )
  ==
  -
  const double(5.)
  )
  ;
  }
  int
  identifier(target_negative_zero_eq)
  (
  void
  )
  {
  return
  const double(0.0)
  ==
  -
  const double(0.0)
  ;
  }
  int
  identifier(target_negative_zero_neq)
  (
  void
  )
  {
  return
  const double(0.0)
  !=
  -
  const double(0.0)
  ;
  }
  int
  identifier(target_negative_zero_lt)
  (
  void
  )
  {
  return
  -
  const double(0.0)
  <
  const double(0.0)
  ;
  }
  int
  identifier(target_negative_zero_gt)
  (
  void
  )
  {
  return
  -
  const double(0.0)
  >
  const double(0.0)
  ;
  }
  int
  identifier(target_negative_zero_ge)
  (
  void
  )
  {
  return
  -
  const double(0.0)
  >=
  const double(0.0)
  ;
  }
  int
  identifier(target_negative_zero_le)
  (
  void
  )
  {
  return
  -
  const double(0.0)
  <=
  const double(0.0)
  ;
  }
  int
  identifier(target_neg_zero_branch)
  (
  void
  )
  {
  if
  (
  -
  const double(0.0)
  )
  {
  return
  const int(1)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(target_not_neg_zero)
  (
  void
  )
  {
  return
  !
  -
  const double(0.0)
  ;
  }
  int
  identifier(target_neg_zero_and)
  (
  void
  )
  {
  return
  -
  const double(0.0)
  &&
  const int(1)
  ;
  }
  double
  identifier(target_add_negative_zero_to_self)
  (
  void
  )
  {
  return
  -
  const double(0.)
  +
  -
  const double(0.)
  ;
  }
  double
  identifier(target_add_positive_to_negative_zero)
  (
  void
  )
  {
  return
  -
  const double(0.)
  +
  const double(0.)
  ;
  }
  double
  identifier(target_add_negative_to_positive_zero)
  (
  void
  )
  {
  return
  const double(0.)
  +
  -
  const double(0.)
  ;
  }
  double
  identifier(target_add_negative_nonzero_to_negative_zero)
  (
  void
  )
  {
  return
  -
  const double(5.0)
  +
  -
  const double(0.)
  ;
  }
  double
  identifier(target_add_positive_nonzero_to_negative_zero)
  (
  void
  )
  {
  return
  -
  const double(0.)
  +
  const double(10.)
  ;
  }
  double
  identifier(target_subtract_neg_zero_from_self)
  (
  void
  )
  {
  return
  -
  const double(0.)
  -
  -
  const double(0.)
  ;
  }
  double
  identifier(target_subtract_pos_zero_from_neg_zero)
  (
  void
  )
  {
  return
  -
  const double(0.)
  -
  const double(0.)
  ;
  }
  double
  identifier(target_subtract_neg_zero_from_pos_zero)
  (
  void
  )
  {
  return
  const double(0.)
  -
  -
  const double(0.)
  ;
  }
  double
  identifier(target_subtract_pos_nonzero_from_neg_zero)
  (
  void
  )
  {
  return
  -
  const double(0.)
  -
  const double(10.)
  ;
  }
  double
  identifier(target_negative_zero_mult)
  (
  void
  )
  {
  return
  -
  const double(0.0)
  *
  const double(15.4e10)
  ;
  }
  double
  identifier(target_negative_zero_mult_negative)
  (
  void
  )
  {
  return
  -
  const double(100.)
  *
  -
  const double(0.)
  ;
  }
  double
  identifier(target_negative_zero_squared)
  (
  void
  )
  {
  return
  -
  const double(0.)
  *
  -
  const double(0.)
  ;
  }
  double
  identifier(target_neg_zero_mult_zero)
  (
  void
  )
  {
  return
  const double(0.)
  *
  -
  const double(0.)
  ;
  }
  double
  identifier(target_mult_underflow)
  (
  void
  )
  {
  return
  -
  const double(5.85543871245623688067e-320)
  *
  const double(0.5e-5)
  ;
  }
  double
  identifier(target_div_neg_zero_by_pos_nonzero)
  (
  void
  )
  {
  return
  -
  const double(0.)
  /
  const double(10.)
  ;
  }
  double
  identifier(target_div_pos_zero_by_neg_nonzero)
  (
  void
  )
  {
  return
  const double(0.)
  /
  -
  const double(10.)
  ;
  }
  double
  identifier(target_div_neg_zero_by_neg_nonzero)
  (
  void
  )
  {
  return
  -
  const double(0.)
  /
  -
  const double(5.)
  ;
  }
  double
  identifier(target_div_negative_underflow)
  (
  void
  )
  {
  return
  const double(0.5e-100)
  /
  -
  const double(2e307)
  ;
  }
  double
  identifier(target_div_pos_non_zero_by_neg_zero)
  (
  void
  )
  {
  return
  const double(10.)
  /
  -
  const double(0.0)
  ;
  }
  double
  identifier(target_div_neg_nonzero_by_zero)
  (
  void
  )
  {
  return
  -
  const double(10.)
  /
  const double(0.)
  ;
  }
  double
  identifier(target_div_neg_nonzero_by_neg_zero)
  (
  void
  )
  {
  return
  -
  const double(100.)
  /
  -
  const double(0.)
  ;
  }
  double
  identifier(target_negate_neg_zero)
  (
  void
  )
  {
  return
  -
  (
  -
  const double(0.0)
  )
  ;
  }
  double
  identifier(target_negate_calculated_zero)
  (
  void
  )
  {
  return
  -
  (
  const double(50.)
  -
  const double(50.)
  )
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
  identifier(target_negative_zero_eq)
  (
  )
  !=
  const int(1)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(target_negative_zero_neq)
  (
  )
  !=
  const int(0)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_negative_zero_lt)
  (
  )
  !=
  const int(0)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_negative_zero_gt)
  (
  )
  !=
  const int(0)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(target_negative_zero_ge)
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
  identifier(target_negative_zero_le)
  (
  )
  !=
  const int(1)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(target_neg_zero_branch)
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
  identifier(target_not_neg_zero)
  (
  )
  !=
  const int(1)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(target_neg_zero_and)
  (
  )
  !=
  const int(0)
  )
  {
  return
  const int(9)
  ;
  }
  double
  identifier(d)
  ;
  identifier(d)
  =
  identifier(target_add_negative_zero_to_self)
  (
  )
  ;
  if
  (
  !
  identifier(is_negative_zero)
  (
  identifier(d)
  )
  )
  {
  return
  const int(10)
  ;
  }
  identifier(d)
  =
  identifier(target_add_positive_to_negative_zero)
  (
  )
  ;
  if
  (
  !
  identifier(is_positive_zero)
  (
  identifier(d)
  )
  )
  {
  return
  const int(11)
  ;
  }
  identifier(d)
  =
  identifier(target_add_negative_to_positive_zero)
  (
  )
  ;
  if
  (
  !
  identifier(is_positive_zero)
  (
  identifier(d)
  )
  )
  {
  return
  const int(12)
  ;
  }
  identifier(d)
  =
  identifier(target_add_negative_nonzero_to_negative_zero)
  (
  )
  ;
  if
  (
  identifier(d)
  !=
  -
  const double(5.)
  )
  {
  return
  const int(13)
  ;
  }
  identifier(d)
  =
  identifier(target_add_positive_nonzero_to_negative_zero)
  (
  )
  ;
  if
  (
  identifier(d)
  !=
  const double(10.)
  )
  {
  return
  const int(14)
  ;
  }
  identifier(d)
  =
  identifier(target_subtract_neg_zero_from_self)
  (
  )
  ;
  if
  (
  !
  identifier(is_positive_zero)
  (
  identifier(d)
  )
  )
  {
  return
  const int(15)
  ;
  }
  identifier(d)
  =
  identifier(target_subtract_pos_zero_from_neg_zero)
  (
  )
  ;
  if
  (
  !
  identifier(is_negative_zero)
  (
  identifier(d)
  )
  )
  {
  return
  const int(16)
  ;
  }
  identifier(d)
  =
  identifier(target_subtract_neg_zero_from_pos_zero)
  (
  )
  ;
  if
  (
  !
  identifier(is_positive_zero)
  (
  identifier(d)
  )
  )
  {
  return
  const int(17)
  ;
  }
  identifier(d)
  =
  identifier(target_subtract_pos_nonzero_from_neg_zero)
  (
  )
  ;
  if
  (
  identifier(d)
  !=
  -
  const double(10.)
  )
  {
  return
  const int(18)
  ;
  }
  identifier(d)
  =
  identifier(target_negative_zero_mult)
  (
  )
  ;
  if
  (
  !
  identifier(is_negative_zero)
  (
  identifier(d)
  )
  )
  {
  return
  const int(19)
  ;
  }
  identifier(d)
  =
  identifier(target_negative_zero_mult_negative)
  (
  )
  ;
  if
  (
  !
  identifier(is_positive_zero)
  (
  identifier(d)
  )
  )
  {
  return
  const int(20)
  ;
  }
  identifier(d)
  =
  identifier(target_negative_zero_squared)
  (
  )
  ;
  if
  (
  !
  identifier(is_positive_zero)
  (
  identifier(d)
  )
  )
  {
  return
  const int(21)
  ;
  }
  identifier(d)
  =
  identifier(target_neg_zero_mult_zero)
  (
  )
  ;
  if
  (
  !
  identifier(is_negative_zero)
  (
  identifier(d)
  )
  )
  {
  return
  const int(22)
  ;
  }
  identifier(d)
  =
  identifier(target_mult_underflow)
  (
  )
  ;
  if
  (
  !
  identifier(is_negative_zero)
  (
  identifier(d)
  )
  )
  {
  return
  const int(23)
  ;
  }
  identifier(d)
  =
  identifier(target_div_neg_zero_by_pos_nonzero)
  (
  )
  ;
  if
  (
  !
  identifier(is_negative_zero)
  (
  identifier(d)
  )
  )
  {
  return
  const int(24)
  ;
  }
  identifier(d)
  =
  identifier(target_div_pos_zero_by_neg_nonzero)
  (
  )
  ;
  if
  (
  !
  identifier(is_negative_zero)
  (
  identifier(d)
  )
  )
  {
  return
  const int(25)
  ;
  }
  identifier(d)
  =
  identifier(target_div_neg_zero_by_neg_nonzero)
  (
  )
  ;
  if
  (
  !
  identifier(is_positive_zero)
  (
  identifier(d)
  )
  )
  {
  return
  const int(26)
  ;
  }
  identifier(d)
  =
  identifier(target_div_pos_non_zero_by_neg_zero)
  (
  )
  ;
  if
  (
  identifier(d)
  >=
  -
  const double(1.79e308)
  )
  {
  return
  const int(27)
  ;
  }
  identifier(d)
  =
  identifier(target_div_neg_nonzero_by_zero)
  (
  )
  ;
  if
  (
  identifier(d)
  >=
  -
  const double(1.79e308)
  )
  {
  return
  const int(28)
  ;
  }
  identifier(d)
  =
  identifier(target_div_neg_nonzero_by_neg_zero)
  (
  )
  ;
  if
  (
  identifier(d)
  <=
  const double(1.79e308)
  )
  {
  return
  const int(29)
  ;
  }
  identifier(d)
  =
  identifier(target_div_negative_underflow)
  (
  )
  ;
  if
  (
  !
  identifier(is_negative_zero)
  (
  identifier(d)
  )
  )
  {
  return
  const int(30)
  ;
  }
  identifier(d)
  =
  identifier(target_negate_neg_zero)
  (
  )
  ;
  if
  (
  !
  identifier(is_positive_zero)
  (
  identifier(d)
  )
  )
  {
  return
  const int(31)
  ;
  }
  identifier(d)
  =
  identifier(target_negate_calculated_zero)
  (
  )
  ;
  if
  (
  !
  identifier(is_negative_zero)
  (
  identifier(d)
  )
  )
  {
  return
  const int(32)
  ;
  }
  return
  const int(0)
  ;
  }
