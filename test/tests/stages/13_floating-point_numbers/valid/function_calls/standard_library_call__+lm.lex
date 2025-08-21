-- Lexing ... OK
+
+
@@ Tokens @@
List[77]:
  double
  identifier(fma)
  (
  double
  identifier(x)
  ,
  double
  identifier(y)
  ,
  double
  identifier(z)
  )
  ;
  double
  identifier(ldexp)
  (
  double
  identifier(x)
  ,
  int
  identifier(exp)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(fma_result)
  =
  identifier(fma)
  (
  const double(5.0)
  ,
  const double(1E22)
  ,
  const double(4000000.0)
  )
  ;
  double
  identifier(ldexp_result)
  =
  identifier(ldexp)
  (
  const double(92E73)
  ,
  const int(5)
  )
  ;
  if
  (
  identifier(fma_result)
  !=
  const double(50000000000000004194304.0)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(ldexp_result)
  !=
  const double(2.944E76)
  )
  {
  return
  const int(2)
  ;
  }
  return
  const int(0)
  ;
  }
