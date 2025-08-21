-- Lexing ... OK
+
+
@@ Tokens @@
List[57]:
  static
  unsigned
  long
  identifier(x)
  =
  const unsigned long(9223372036854775803ul)
  ;
  unsigned
  long
  identifier(zero_long)
  ;
  unsigned
  identifier(zero_int)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(x)
  !=
  const unsigned long(9223372036854775803ul)
  )
  return
  const int(0)
  ;
  identifier(x)
  =
  identifier(x)
  +
  const int(10)
  ;
  if
  (
  identifier(x)
  !=
  const unsigned long(9223372036854775813ul)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(zero_long)
  ||
  identifier(zero_int)
  )
  return
  const int(0)
  ;
  return
  const int(1)
  ;
  }
