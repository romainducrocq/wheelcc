-- Lexing ... OK
+
+
@@ Tokens @@
List[233]:
  double
  identifier(zero)
  =
  const double(0.0)
  ;
  double
  identifier(non_zero)
  =
  const double(1E-20)
  ;
  double
  identifier(one)
  =
  const double(1.0)
  ;
  double
  identifier(rounded_to_zero)
  =
  const double(1e-330)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(zero)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(rounded_to_zero)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(non_zero)
  )
  {
  }
  else
  {
  return
  const int(3)
  ;
  }
  if
  (
  const double(0.e10)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  !
  identifier(non_zero)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  !
  (
  !
  identifier(zero)
  )
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  !
  (
  !
  identifier(rounded_to_zero)
  )
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  !
  (
  identifier(non_zero)
  &&
  const double(1.0)
  )
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  const double(3.0)
  &&
  identifier(zero)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(rounded_to_zero)
  &&
  const double(1000e10)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  const unsigned long(18446744073709551615UL)
  &&
  identifier(zero)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  !
  (
  identifier(non_zero)
  &&
  const long(5l)
  )
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  !
  (
  const double(5.0)
  ||
  identifier(zero)
  )
  )
  {
  return
  const int(12)
  ;
  }
  if
  (
  identifier(zero)
  ||
  identifier(rounded_to_zero)
  )
  {
  return
  const int(13)
  ;
  }
  if
  (
  !
  (
  identifier(rounded_to_zero)
  ||
  const double(0.0001)
  )
  )
  {
  return
  const int(14)
  ;
  }
  if
  (
  !
  (
  identifier(non_zero)
  ||
  const unsigned int(0u)
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
  const int(0)
  ||
  const double(0.0000005)
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
