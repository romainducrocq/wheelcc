-- Lexing ... OK
+
+
@@ Tokens @@
List[53]:
  int
  identifier(callee)
  (
  void
  )
  {
  return
  const int(0)
  ;
  }
  int
  identifier(target)
  (
  int
  identifier(x)
  )
  {
  switch
  (
  identifier(x)
  )
  {
  return
  identifier(callee)
  (
  )
  ;
  case
  const int(1)
  :
  return
  const int(1)
  ;
  default
  :
  return
  const int(2)
  ;
  }
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
  const int(1)
  )
  ;
  }
