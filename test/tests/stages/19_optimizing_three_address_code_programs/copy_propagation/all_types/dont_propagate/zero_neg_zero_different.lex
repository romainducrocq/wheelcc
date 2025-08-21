-- Lexing ... OK
+
+
@@ Tokens @@
List[69]:
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
  identifier(target)
  (
  int
  identifier(flag)
  )
  {
  double
  identifier(result)
  =
  const double(0.0)
  ;
  if
  (
  identifier(flag)
  )
  {
  identifier(result)
  =
  -
  const double(0.0)
  ;
  }
  return
  identifier(result)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(pos_inf)
  =
  const int(1)
  /
  identifier(target)
  (
  const int(0)
  )
  ;
  double
  identifier(neg_inf)
  =
  const int(1)
  /
  identifier(target)
  (
  const int(1)
  )
  ;
  return
  identifier(pos_inf)
  >
  identifier(neg_inf)
  ;
  }
