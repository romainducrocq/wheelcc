-- Lexing ... OK
+
+
@@ Tokens @@
List[94]:
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
  void
  )
  {
  static
  int
  identifier(x)
  =
  const int(3)
  ;
  static
  int
  identifier(y)
  =
  const int(0)
  ;
  identifier(y)
  =
  identifier(x)
  ;
  int
  identifier(sum)
  =
  identifier(callee)
  (
  identifier(x)
  ,
  identifier(y)
  )
  ;
  identifier(x)
  =
  identifier(x)
  +
  const int(1)
  ;
  return
  identifier(sum)
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
  )
  !=
  const int(6)
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
  )
  !=
  const int(8)
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
