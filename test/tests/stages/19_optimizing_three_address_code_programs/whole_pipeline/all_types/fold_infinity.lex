-- Lexing ... OK
+
+
@@ Tokens @@
List[636]:
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
  identifier(target_infinity_equal)
  (
  void
  )
  {
  return
  const double(1.0)
  /
  const double(0.0)
  ==
  const double(11e330)
  ;
  }
  int
  identifier(target_infinity_gt)
  (
  void
  )
  {
  return
  const double(1.0)
  /
  const double(0.0)
  >
  const double(1.79E308)
  ;
  }
  int
  identifier(target_neg_infinity_lt)
  (
  void
  )
  {
  return
  -
  const double(1.0)
  /
  const double(0.0)
  <
  -
  const double(1.79E308)
  ;
  }
  double
  identifier(target_add_infinity)
  (
  void
  )
  {
  return
  (
  const double(1.0)
  /
  const double(0.0)
  )
  +
  const double(1000e10)
  ;
  }
  double
  identifier(target_sub_infinity)
  (
  void
  )
  {
  return
  (
  const double(1.0)
  /
  const double(0.0)
  )
  -
  const double(1000e10)
  ;
  }
  double
  identifier(target_mult_infinity)
  (
  void
  )
  {
  return
  (
  const double(1.0)
  /
  const double(0.0)
  )
  *
  const double(25.)
  ;
  }
  double
  identifier(target_mult_neg_infinity)
  (
  void
  )
  {
  return
  (
  -
  const double(1.0)
  /
  const double(0.0)
  )
  *
  const double(25.)
  ;
  }
  double
  identifier(target_mult_neg_infinity_by_neg_number)
  (
  void
  )
  {
  return
  (
  -
  const double(1.0)
  /
  const double(0.0)
  )
  *
  -
  const double(25.)
  ;
  }
  double
  identifier(target_div_infinity_by_num)
  (
  void
  )
  {
  return
  (
  const double(1.)
  /
  const double(0.)
  )
  /
  const double(5.)
  ;
  }
  double
  identifier(target_negate_inf)
  (
  void
  )
  {
  double
  identifier(infin)
  =
  const double(1.0)
  /
  const double(0.0)
  ;
  return
  -
  identifier(infin)
  ;
  }
  double
  identifier(target_mult_overflow)
  (
  void
  )
  {
  return
  const double(2e300)
  *
  const double(10e20)
  ;
  }
  double
  identifier(target_div_overflow)
  (
  void
  )
  {
  return
  const double(1e308)
  /
  -
  const double(10e-20)
  ;
  }
  double
  identifier(target_add_overflow)
  (
  void
  )
  {
  return
  const double(1.79e308)
  +
  const double(1e308)
  ;
  }
  double
  identifier(target_sub_overflow)
  (
  void
  )
  {
  return
  -
  const double(1e308)
  -
  const double(1e308)
  ;
  }
  double
  identifier(target_zero_div_by_inf)
  (
  void
  )
  {
  return
  const double(0.)
  /
  (
  const double(3.0)
  /
  const double(0.)
  )
  ;
  }
  double
  identifier(target_zero_div_by_neg_inf)
  (
  void
  )
  {
  return
  const double(0.)
  /
  (
  const double(3.0)
  /
  -
  const double(0.)
  )
  ;
  }
  double
  identifier(target_negative_zero_div_by_inf)
  (
  void
  )
  {
  return
  -
  const double(0.)
  /
  (
  const double(0.005)
  /
  const double(0.)
  )
  ;
  }
  int
  identifier(target_infinity_is_true)
  (
  void
  )
  {
  double
  identifier(infin)
  =
  const double(2345.0)
  /
  const double(0.0)
  ;
  if
  (
  identifier(infin)
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
  double
  identifier(zero)
  =
  const double(0.)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(infinity)
  =
  const double(1.)
  /
  identifier(zero)
  ;
  if
  (
  !
  identifier(target_infinity_equal)
  (
  )
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  !
  identifier(target_infinity_gt)
  (
  )
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  !
  identifier(target_neg_infinity_lt)
  (
  )
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_add_infinity)
  (
  )
  !=
  identifier(infinity)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(target_sub_infinity)
  (
  )
  !=
  identifier(infinity)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(target_mult_infinity)
  (
  )
  !=
  identifier(infinity)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(target_mult_neg_infinity)
  (
  )
  !=
  -
  identifier(infinity)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(target_mult_neg_infinity_by_neg_number)
  (
  )
  !=
  identifier(infinity)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(target_div_infinity_by_num)
  (
  )
  !=
  identifier(infinity)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(target_negate_inf)
  (
  )
  !=
  -
  identifier(infinity)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  identifier(target_mult_overflow)
  (
  )
  !=
  identifier(infinity)
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  identifier(target_div_overflow)
  (
  )
  !=
  -
  identifier(infinity)
  )
  {
  return
  const int(12)
  ;
  }
  if
  (
  identifier(target_add_overflow)
  (
  )
  !=
  identifier(infinity)
  )
  {
  return
  const int(13)
  ;
  }
  if
  (
  identifier(target_sub_overflow)
  (
  )
  !=
  -
  identifier(infinity)
  )
  {
  return
  const int(14)
  ;
  }
  double
  identifier(d)
  =
  identifier(target_zero_div_by_inf)
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
  identifier(target_zero_div_by_neg_inf)
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
  identifier(target_negative_zero_div_by_inf)
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
  const int(17)
  ;
  }
  if
  (
  identifier(target_infinity_is_true)
  (
  )
  !=
  const int(1)
  )
  {
  return
  const int(18)
  ;
  }
  return
  const int(0)
  ;
  }
