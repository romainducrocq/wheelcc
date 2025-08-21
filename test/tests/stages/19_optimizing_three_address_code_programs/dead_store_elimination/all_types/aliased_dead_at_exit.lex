-- Lexing ... OK
+
+
@@ Tokens @@
List[93]:
  int
  identifier(b)
  =
  const int(0)
  ;
  void
  identifier(callee)
  (
  int
  *
  identifier(ptr)
  )
  {
  identifier(b)
  =
  *
  identifier(ptr)
  ;
  *
  identifier(ptr)
  =
  const int(100)
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
  const int(10)
  ;
  identifier(callee)
  (
  &
  identifier(x)
  )
  ;
  int
  identifier(y)
  =
  identifier(x)
  ;
  identifier(x)
  =
  const int(50)
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
  int
  identifier(a)
  =
  identifier(target)
  (
  )
  ;
  if
  (
  identifier(a)
  !=
  const int(100)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(b)
  !=
  const int(10)
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
