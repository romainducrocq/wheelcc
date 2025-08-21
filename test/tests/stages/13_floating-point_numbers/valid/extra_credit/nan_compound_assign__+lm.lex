-- Lexing ... OK
+
+
@@ Tokens @@
List[90]:
  int
  identifier(double_isnan)
  (
  double
  identifier(d)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  static
  double
  identifier(zero)
  =
  const double(0.0)
  ;
  double
  identifier(nan)
  =
  const double(0.0)
  /
  identifier(zero)
  ;
  if
  (
  !
  identifier(double_isnan)
  (
  identifier(nan)
  +=
  const double(99.2)
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
  identifier(double_isnan)
  (
  identifier(nan)
  -=
  identifier(nan)
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
  identifier(double_isnan)
  (
  identifier(nan)
  *=
  const double(4.0)
  )
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  !
  identifier(double_isnan)
  (
  identifier(nan)
  /=
  const double(0.0)
  )
  )
  {
  return
  const int(4)
  ;
  }
  return
  const int(0)
  ;
  }
