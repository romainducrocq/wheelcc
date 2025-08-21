-- Lexing ... OK
+
+
@@ Tokens @@
List[110]:
  double
  identifier(inf)
  =
  const double(2e308)
  ;
  double
  identifier(very_large)
  =
  const double(1.79E308)
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
  if
  (
  identifier(inf)
  !=
  const double(11e330)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(inf)
  <=
  identifier(very_large)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(very_large)
  *
  const double(10.0)
  !=
  identifier(inf)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  const double(1.0)
  /
  identifier(zero)
  !=
  identifier(inf)
  )
  {
  return
  const int(4)
  ;
  }
  double
  identifier(negated_inf)
  =
  -
  identifier(inf)
  ;
  double
  identifier(negated_inf2)
  =
  -
  const double(1.0)
  /
  identifier(zero)
  ;
  if
  (
  identifier(negated_inf)
  >=
  -
  identifier(very_large)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(negated_inf)
  !=
  identifier(negated_inf2)
  )
  {
  return
  const int(6)
  ;
  }
  return
  const int(0)
  ;
  }
