-- Lexing ... OK
+
+
@@ Tokens @@
List[65]:
  int
  identifier(callee)
  (
  void
  )
  {
  return
  const int(1)
  /
  const int(0)
  ;
  }
  int
  identifier(target)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(5)
  ;
  return
  identifier(x)
  ;
  for
  (
  ;
  identifier(x)
  <
  const int(10)
  ;
  identifier(x)
  =
  identifier(x)
  +
  const int(1)
  )
  {
  identifier(x)
  =
  identifier(x)
  +
  identifier(callee)
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
  return
  identifier(target)
  (
  )
  ;
  }
