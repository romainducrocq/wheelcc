-- Lexing ... OK
+
+
@@ Tokens @@
List[78]:
  extern
  long
  int
  identifier(l)
  ;
  long
  identifier(return_l)
  (
  void
  )
  ;
  int
  identifier(return_l_as_int)
  (
  void
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(return_l)
  (
  )
  !=
  const long(8589934592l)
  )
  return
  const int(1)
  ;
  if
  (
  identifier(return_l_as_int)
  (
  )
  !=
  const int(0)
  )
  return
  const int(2)
  ;
  identifier(l)
  =
  identifier(l)
  -
  const long(10l)
  ;
  if
  (
  identifier(return_l)
  (
  )
  !=
  const long(8589934582l)
  )
  return
  const int(3)
  ;
  if
  (
  identifier(return_l_as_int)
  (
  )
  !=
  -
  const int(10)
  )
  return
  const int(4)
  ;
  return
  const int(0)
  ;
  }
