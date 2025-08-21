-- Lexing ... OK
+
+
@@ Tokens @@
List[407]:
  double
  identifier(target_add)
  (
  void
  )
  {
  return
  const double(1.2345e60)
  +
  const double(1.)
  ;
  }
  double
  identifier(target_sub)
  (
  void
  )
  {
  return
  const double(5.85543871245623688067e-311)
  -
  const double(5.85543871245574281503e-311)
  ;
  }
  double
  identifier(target_mult)
  (
  void
  )
  {
  return
  const double(2.1)
  *
  const double(3.0)
  ;
  }
  double
  identifier(target_div)
  (
  void
  )
  {
  return
  const double(1100.5)
  /
  const double(5000.)
  ;
  }
  double
  identifier(target_div_underflow)
  (
  void
  )
  {
  return
  const double(0.5e-100)
  /
  const double(2e307)
  ;
  }
  double
  identifier(target_neg)
  (
  void
  )
  {
  return
  -
  const double(.000000275)
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
  const double(1e30)
  ;
  }
  int
  identifier(target_eq)
  (
  void
  )
  {
  return
  const double(0.1)
  ==
  const double(0.10000000000000001)
  ;
  }
  int
  identifier(target_neq)
  (
  void
  )
  {
  return
  const double(5e-324)
  !=
  const double(0.0)
  ;
  }
  int
  identifier(target_gt)
  (
  void
  )
  {
  return
  const double(1e308)
  >
  const double(1e307)
  ;
  }
  int
  identifier(target_ge)
  (
  void
  )
  {
  return
  const double(3.1)
  >=
  const double(3.1)
  ;
  }
  int
  identifier(target_lt)
  (
  void
  )
  {
  return
  const double(0.1)
  <
  const double(0.10000000000000001)
  ;
  }
  int
  identifier(target_le)
  (
  void
  )
  {
  return
  const double(0.5)
  <=
  const double(0.)
  ;
  }
  double
  identifier(target_negate_zero)
  (
  void
  )
  {
  return
  -
  const double(0.0)
  ;
  }
  double
  identifier(target_infinity)
  (
  void
  )
  {
  return
  const double(1e308)
  *
  const double(2.)
  ;
  }
  int
  identifier(target_compare_infininty)
  (
  void
  )
  {
  return
  const double(10e308)
  ==
  const double(12e308)
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
  const double(1.2345e60)
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
  const double(5e-324)
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
  const double(6.300000000000001)
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
  const double(0.2201)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(target_div_underflow)
  (
  )
  !=
  const double(0.0)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(target_neg)
  (
  )
  +
  const double(.000000275)
  !=
  const double(0.0)
  )
  {
  return
  const int(6)
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
  const int(7)
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
  const int(8)
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
  const int(9)
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
  const int(10)
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
  const int(11)
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
  const int(12)
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
  const int(13)
  ;
  }
  if
  (
  identifier(target_infinity)
  (
  )
  !=
  const double(10e308)
  )
  {
  return
  const int(14)
  ;
  }
  if
  (
  !
  identifier(target_compare_infininty)
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
  !
  (
  (
  const int(1)
  /
  identifier(target_negate_zero)
  (
  )
  )
  <
  const double(0.0)
  )
  )
  {
  return
  const int(16)
  ;
  }
  return
  const int(0)
  ;
  }
