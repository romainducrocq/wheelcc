-- Lexing ... OK
+
+
@@ Tokens @@
List[94]:
  int
  identifier(var)
  =
  const int(0)
  ;
  int
  identifier(callee)
  (
  void
  )
  {
  identifier(var)
  =
  identifier(var)
  +
  const int(1)
  ;
  return
  const int(0)
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
  const int(3)
  ;
  if
  (
  identifier(flag)
  )
  identifier(callee)
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
  const int(0)
  )
  !=
  const int(3)
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
  const int(1)
  )
  !=
  const int(3)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(var)
  !=
  const int(1)
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
