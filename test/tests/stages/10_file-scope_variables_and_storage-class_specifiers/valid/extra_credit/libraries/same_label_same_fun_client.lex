-- Lexing ... OK
+
+
@@ Tokens @@
List[60]:
  int
  identifier(f)
  (
  void
  )
  {
  goto
  identifier(x)
  ;
  return
  const int(0)
  ;
  identifier(x)
  :
  return
  const int(1)
  ;
  }
  int
  identifier(f_caller)
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
  if
  (
  identifier(f)
  (
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
  identifier(f_caller)
  (
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
