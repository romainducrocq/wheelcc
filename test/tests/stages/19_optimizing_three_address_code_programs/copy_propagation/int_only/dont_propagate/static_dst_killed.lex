-- Lexing ... OK
+
+
@@ Tokens @@
List[69]:
  int
  identifier(x)
  ;
  int
  identifier(update_x)
  (
  void
  )
  {
  identifier(x)
  =
  const int(4)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(target)
  (
  void
  )
  {
  identifier(x)
  =
  const int(3)
  ;
  identifier(update_x)
  (
  )
  ;
  return
  identifier(x)
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
  identifier(target)
  (
  )
  !=
  const int(4)
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
  const int(4)
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
