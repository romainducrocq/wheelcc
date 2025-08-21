-- Lexing ... OK
+
+
@@ Tokens @@
List[64]:
  int
  identifier(flag)
  =
  const int(1)
  ;
  int
  identifier(target)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(4)
  ;
  int
  identifier(z)
  ;
  if
  (
  const int(4)
  -
  identifier(x)
  )
  {
  identifier(x)
  =
  const int(3)
  ;
  }
  if
  (
  !
  identifier(flag)
  )
  {
  identifier(z)
  =
  const int(10)
  ;
  }
  identifier(z)
  =
  identifier(x)
  +
  const int(5)
  ;
  return
  identifier(z)
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
