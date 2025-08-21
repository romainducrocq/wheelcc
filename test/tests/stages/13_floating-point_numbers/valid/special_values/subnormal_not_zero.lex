-- Lexing ... OK
+
+
@@ Tokens @@
List[57]:
  int
  identifier(non_zero)
  (
  double
  identifier(d)
  )
  {
  return
  !
  identifier(d)
  ;
  }
  double
  identifier(multiply_by_large_num)
  (
  double
  identifier(d)
  )
  {
  return
  identifier(d)
  *
  const double(2e20)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(subnormal)
  =
  const double(2.5e-320)
  ;
  if
  (
  identifier(multiply_by_large_num)
  (
  identifier(subnormal)
  )
  !=
  const double(4.99994433591341498562e-300)
  )
  {
  return
  const int(2)
  ;
  }
  return
  identifier(non_zero)
  (
  identifier(subnormal)
  )
  ;
  }
