-- Lexing ... OK
+
+
@@ Tokens @@
List[44]:
  int
  identifier(double_isnan)
  (
  double
  identifier(d)
  )
  ;
  double
  identifier(target_nan)
  (
  void
  )
  {
  return
  const double(0.)
  /
  const double(0.)
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
  !
  identifier(double_isnan)
  (
  identifier(target_nan)
  (
  )
  )
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
