-- Lexing ... OK
+
+
@@ Tokens @@
List[100]:
  int
  identifier(callee)
  (
  int
  identifier(arg)
  )
  {
  return
  identifier(arg)
  *
  const int(2)
  ;
  }
  int
  identifier(target)
  (
  int
  identifier(arg)
  ,
  int
  identifier(flag)
  )
  {
  int
  identifier(x)
  =
  identifier(arg)
  +
  const int(1)
  ;
  if
  (
  identifier(flag)
  )
  {
  identifier(x)
  =
  identifier(arg)
  -
  const int(1)
  ;
  }
  int
  identifier(y)
  =
  identifier(callee)
  (
  identifier(x)
  )
  ;
  identifier(x)
  =
  const int(100)
  ;
  return
  identifier(y)
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
  const int(4)
  ,
  const int(0)
  )
  !=
  const int(10)
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
  const int(3)
  ,
  const int(1)
  )
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
