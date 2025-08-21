-- Lexing ... OK
+
+
@@ Tokens @@
List[152]:
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
  identifier(zero)
  =
  const double(0.0)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(negative_zero)
  =
  -
  identifier(zero)
  ;
  if
  (
  identifier(negative_zero)
  !=
  const int(0)
  )
  return
  const int(1)
  ;
  if
  (
  const int(1)
  /
  identifier(negative_zero)
  !=
  -
  const double(10e308)
  )
  return
  const int(2)
  ;
  if
  (
  (
  -
  const int(10)
  )
  /
  identifier(negative_zero)
  !=
  const double(10e308)
  )
  return
  const int(3)
  ;
  int
  identifier(fail)
  =
  const int(0)
  ;
  identifier(negative_zero)
  &&
  (
  identifier(fail)
  =
  const int(1)
  )
  ;
  if
  (
  identifier(fail)
  )
  return
  const int(4)
  ;
  if
  (
  identifier(negative_zero)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(zero)
  !=
  -
  const double(0.0)
  )
  {
  return
  const int(6)
  ;
  }
  double
  identifier(negated)
  =
  identifier(copysign)
  (
  const double(4.0)
  ,
  -
  const double(0.0)
  )
  ;
  double
  identifier(positive)
  =
  identifier(copysign)
  (
  -
  const double(5.0)
  ,
  const double(0.0)
  )
  ;
  if
  (
  identifier(negated)
  !=
  -
  const double(4.0)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(positive)
  !=
  const double(5.0)
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
