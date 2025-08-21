-- Lexing ... OK
+
+
@@ Tokens @@
List[81]:
  static
  int
  identifier(x)
  =
  const int(1)
  ;
  int
  identifier(read_internal_x)
  (
  void
  )
  ;
  int
  identifier(read_x)
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
  extern
  int
  identifier(x)
  ;
  if
  (
  identifier(x)
  !=
  const int(1)
  )
  return
  const int(1)
  ;
  identifier(x)
  =
  const int(2)
  ;
  if
  (
  identifier(read_internal_x)
  (
  )
  !=
  const int(2)
  )
  return
  const int(1)
  ;
  if
  (
  identifier(read_x)
  (
  )
  !=
  const int(10)
  )
  return
  const int(1)
  ;
  return
  const int(0)
  ;
  }
  extern
  int
  identifier(x)
  ;
  int
  identifier(read_internal_x)
  (
  void
  )
  {
  return
  identifier(x)
  ;
  }
