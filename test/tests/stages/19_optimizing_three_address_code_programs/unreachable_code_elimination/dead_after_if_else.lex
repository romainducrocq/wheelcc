-- Lexing ... OK
+
+
@@ Tokens @@
List[76]:
  int
  identifier(callee)
  (
  void
  )
  {
  return
  const int(100)
  ;
  }
  int
  identifier(target)
  (
  int
  identifier(a)
  )
  {
  if
  (
  identifier(a)
  )
  {
  return
  const int(1)
  ;
  }
  else
  {
  return
  const int(2)
  ;
  }
  return
  identifier(callee)
  (
  )
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
  const int(1)
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
  const int(2)
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
