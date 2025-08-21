-- Lexing ... OK
+
+
@@ Tokens @@
List[108]:
  int
  identifier(callee)
  (
  int
  identifier(a)
  ,
  int
  identifier(b)
  )
  {
  return
  identifier(a)
  +
  identifier(b)
  ;
  }
  int
  identifier(target)
  (
  int
  identifier(flag)
  )
  {
  static
  int
  identifier(x)
  ;
  static
  int
  identifier(y)
  ;
  if
  (
  identifier(flag)
  )
  {
  identifier(y)
  =
  const int(20)
  ;
  identifier(x)
  =
  identifier(y)
  ;
  }
  else
  {
  identifier(y)
  =
  const int(100)
  ;
  identifier(x)
  =
  identifier(y)
  ;
  }
  return
  identifier(callee)
  (
  identifier(x)
  ,
  identifier(y)
  )
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
  const int(0)
  )
  ;
  if
  (
  identifier(result)
  !=
  const int(200)
  )
  return
  const int(1)
  ;
  identifier(result)
  =
  identifier(target)
  (
  const int(1)
  )
  ;
  if
  (
  identifier(result)
  !=
  const int(40)
  )
  return
  const int(2)
  ;
  return
  const int(0)
  ;
  }
