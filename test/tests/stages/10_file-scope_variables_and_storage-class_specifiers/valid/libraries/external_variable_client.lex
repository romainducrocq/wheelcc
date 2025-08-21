-- Lexing ... OK
+
+
@@ Tokens @@
List[96]:
  int
  identifier(update_x)
  (
  int
  identifier(new_val)
  )
  ;
  int
  identifier(read_x)
  (
  void
  )
  ;
  extern
  int
  identifier(x)
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
  const int(3)
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
  const int(3)
  )
  return
  const int(1)
  ;
  identifier(x)
  =
  const int(4)
  ;
  if
  (
  identifier(x)
  !=
  const int(4)
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
  const int(4)
  )
  return
  const int(1)
  ;
  identifier(update_x)
  (
  const int(5)
  )
  ;
  if
  (
  identifier(x)
  !=
  const int(5)
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
  return
  const int(0)
  ;
  }
