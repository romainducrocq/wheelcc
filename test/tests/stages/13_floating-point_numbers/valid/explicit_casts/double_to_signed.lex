-- Lexing ... OK
+
+
@@ Tokens @@
List[78]:
  int
  identifier(double_to_int)
  (
  double
  identifier(d)
  )
  {
  return
  (
  int
  )
  identifier(d)
  ;
  }
  long
  identifier(double_to_long)
  (
  double
  identifier(d)
  )
  {
  return
  (
  long
  )
  identifier(d)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  long
  identifier(l)
  =
  identifier(double_to_long)
  (
  const double(2148429099.3)
  )
  ;
  if
  (
  identifier(l)
  !=
  const long(2148429099l)
  )
  {
  return
  const int(1)
  ;
  }
  int
  identifier(i)
  =
  identifier(double_to_int)
  (
  -
  const double(200000.9999)
  )
  ;
  if
  (
  identifier(i)
  !=
  -
  const int(200000)
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
