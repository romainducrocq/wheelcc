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
  int
  identifier(dead_int_cast)
  =
  const int(0)
  ?
  (
  int
  )
  const double(2147483649.0)
  :
  const int(100)
  ;
  unsigned
  int
  identifier(dead_uint_cast)
  =
  const int(0)
  ?
  (
  unsigned
  int
  )
  const double(34359738368.0)
  :
  const int(200)
  ;
  signed
  long
  identifier(dead_long_cast)
  =
  const int(1)
  ?
  const int(300)
  :
  const double(9223372036854777856.0)
  ;
  unsigned
  long
  identifier(dead_ulong_cast)
  =
  const int(1)
  ?
  const int(200)
  :
  (
  unsigned
  long
  )
  const double(200e300)
  ;
  return
  identifier(dead_int_cast)
  +
  identifier(dead_uint_cast)
  +
  identifier(dead_long_cast)
  +
  identifier(dead_ulong_cast)
  ;
  }
