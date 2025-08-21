-- Lexing ... OK
+
+
@@ Tokens @@
List[100]:
  int
  identifier(x)
  =
  const int(0)
  ;
  int
  identifier(y)
  =
  const int(0)
  ;
  int
  identifier(callee)
  (
  void
  )
  {
  identifier(y)
  =
  identifier(x)
  *
  const int(2)
  ;
  return
  const int(5)
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
  const int(2)
  ;
  identifier(x)
  =
  identifier(callee)
  (
  )
  ;
  identifier(x)
  =
  const int(2)
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
  int
  identifier(result)
  =
  identifier(target)
  (
  )
  ;
  if
  (
  identifier(result)
  !=
  const int(2)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(y)
  !=
  const int(4)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(x)
  !=
  const int(2)
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
