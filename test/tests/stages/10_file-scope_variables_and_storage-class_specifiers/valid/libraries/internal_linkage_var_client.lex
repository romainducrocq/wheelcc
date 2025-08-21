-- Lexing ... OK
+
+
@@ Tokens @@
List[108]:
  static
  int
  identifier(x)
  ;
  static
  int
  identifier(x)
  ;
  int
  identifier(read_x)
  (
  void
  )
  ;
  int
  identifier(update_x)
  (
  int
  identifier(x)
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
  identifier(x)
  !=
  const int(0)
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
  const int(5)
  )
  return
  const int(1)
  ;
  extern
  int
  identifier(x)
  ;
  identifier(update_x)
  (
  const int(10)
  )
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
  if
  (
  identifier(x)
  !=
  const int(0)
  )
  return
  const int(1)
  ;
  identifier(x)
  =
  const int(20)
  ;
  if
  (
  identifier(x)
  !=
  const int(20)
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
  static
  int
  identifier(x)
  ;
