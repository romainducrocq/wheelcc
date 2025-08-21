-- Lexing ... OK
+
+
@@ Tokens @@
List[66]:
  int
  identifier(main)
  (
  void
  )
  {
  long
  identifier(x)
  =
  -
  const long(9223372036854775807l)
  ;
  if
  (
  identifier(x)
  ++
  !=
  -
  const long(9223372036854775807l)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(x)
  !=
  -
  const long(9223372036854775806l)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  --
  identifier(x)
  !=
  -
  const long(9223372036854775807l)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(x)
  !=
  -
  const long(9223372036854775807l)
  )
  {
  return
  const int(4)
  ;
  }
  return
  const int(0)
  ;
  }
