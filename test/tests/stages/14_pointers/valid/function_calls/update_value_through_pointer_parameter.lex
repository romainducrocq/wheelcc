-- Lexing ... OK
+
+
@@ Tokens @@
List[69]:
  int
  identifier(update_value)
  (
  int
  *
  identifier(ptr)
  )
  {
  int
  identifier(old_val)
  =
  *
  identifier(ptr)
  ;
  *
  identifier(ptr)
  =
  const int(10)
  ;
  return
  identifier(old_val)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(20)
  ;
  int
  identifier(result)
  =
  identifier(update_value)
  (
  &
  identifier(x)
  )
  ;
  if
  (
  identifier(result)
  !=
  const int(20)
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
  const int(10)
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
