-- Lexing ... OK
+
+
@@ Tokens @@
List[200]:
  long
  identifier(target_remainder_test)
  (
  void
  )
  {
  return
  -
  const long(8589934585l)
  %
  const long(4294967290l)
  ;
  }
  long
  identifier(target_long_subtraction)
  (
  void
  )
  {
  return
  -
  const long(4294967290l)
  -
  const long(90l)
  ;
  }
  long
  identifier(target_long_division)
  (
  void
  )
  {
  return
  (
  -
  const long(4294967290l)
  /
  const long(128l)
  )
  ;
  }
  long
  identifier(target_long_complement)
  (
  void
  )
  {
  return
  ~
  -
  const long(9223372036854775807l)
  ;
  }
  double
  identifier(target_double_add)
  (
  void
  )
  {
  return
  -
  const double(1.2345e60)
  +
  const double(1.)
  ;
  }
  double
  identifier(target_double_sub)
  (
  void
  )
  {
  return
  -
  const double(5.85543871245623688067e-311)
  -
  -
  const double(5.85543871245574281503e-311)
  ;
  }
  double
  identifier(target_double_div)
  (
  void
  )
  {
  return
  -
  const double(1100.5)
  /
  const double(5000.)
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
  identifier(target_remainder_test)
  (
  )
  !=
  -
  const long(5l)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(target_long_subtraction)
  (
  )
  !=
  -
  const long(4294967380l)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_long_division)
  (
  )
  !=
  -
  const long(33554431l)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_long_complement)
  (
  )
  !=
  const long(9223372036854775806l)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(target_double_add)
  (
  )
  !=
  -
  const double(1.2345e60)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(target_double_sub)
  (
  )
  !=
  -
  const double(5e-324)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(target_double_div)
  (
  )
  !=
  -
  const double(0.2201)
  )
  {
  return
  const int(7)
  ;
  }
  return
  const int(0)
  ;
  }
