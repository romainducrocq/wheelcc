-- Lexing ... OK
+
+
@@ Tokens @@
List[72]:
  int
  identifier(double_isnan)
  (
  double
  identifier(d)
  )
  ;
  double
  identifier(na)
  ;
  int
  identifier(target)
  (
  int
  identifier(flag)
  )
  {
  identifier(na)
  =
  const double(0.0)
  /
  const double(0.0)
  ;
  double
  identifier(d)
  =
  const double(0.0)
  /
  const double(0.0)
  ;
  if
  (
  identifier(flag)
  )
  {
  identifier(na)
  =
  identifier(d)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  identifier(target)
  (
  const int(1)
  )
  ;
  if
  (
  !
  identifier(double_isnan)
  (
  identifier(na)
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
