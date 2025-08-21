-- Lexing ... OK
+
+
@@ Tokens @@
List[95]:
  int
  identifier(callee)
  (
  void
  )
  {
  return
  const int(4)
  ;
  }
  int
  identifier(callee2)
  (
  void
  )
  {
  return
  const int(5)
  ;
  }
  int
  identifier(target)
  (
  int
  identifier(flag)
  )
  {
  int
  identifier(x)
  =
  const int(10)
  ;
  if
  (
  identifier(flag)
  )
  {
  identifier(x)
  =
  identifier(callee)
  (
  )
  ;
  }
  else
  {
  identifier(x)
  =
  identifier(callee2)
  (
  )
  ;
  }
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
  const int(1)
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
  identifier(target)
  (
  const int(0)
  )
  !=
  const int(5)
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
