-- Lexing ... OK
+
+
@@ Tokens @@
List[89]:
  double
  identifier(int_to_double)
  (
  int
  identifier(i)
  )
  {
  return
  (
  double
  )
  identifier(i)
  ;
  }
  double
  identifier(long_to_double)
  (
  long
  identifier(l)
  )
  {
  return
  (
  double
  )
  identifier(l)
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
  identifier(int_to_double)
  (
  -
  const int(100000)
  )
  !=
  -
  const double(100000.0)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(long_to_double)
  (
  -
  const long(9007199254751227l)
  )
  !=
  -
  const double(9007199254751228.0)
  )
  {
  return
  const int(2)
  ;
  }
  double
  identifier(d)
  =
  (
  double
  )
  const long(1152921504606846977l)
  ;
  if
  (
  identifier(d)
  !=
  const double(1152921504606846976.0)
  )
  {
  return
  const int(3)
  ;
  }
  return
  const int(0)
  ;
  }
