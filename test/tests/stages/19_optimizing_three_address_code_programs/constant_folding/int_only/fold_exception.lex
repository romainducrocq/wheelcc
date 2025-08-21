-- Lexing ... OK
+
+
@@ Tokens @@
List[78]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(dead_div_by_zero)
  =
  const int(1)
  ||
  (
  const int(1)
  /
  const int(0)
  )
  ;
  int
  identifier(dead_zero_remainder)
  =
  const int(0)
  &&
  (
  const int(100)
  %
  const int(0)
  )
  ;
  int
  identifier(overflow)
  =
  const int(0)
  ?
  (
  const int(2147483647)
  +
  const int(10)
  )
  :
  const int(100)
  ;
  if
  (
  identifier(dead_div_by_zero)
  !=
  const int(1)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(dead_zero_remainder)
  !=
  const int(0)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(overflow)
  !=
  const int(100)
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
