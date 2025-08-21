-- Lexing ... OK
+
+
@@ Tokens @@
List[52]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(foo)
  (
  void
  )
  ;
  int
  identifier(x)
  =
  identifier(foo)
  (
  )
  ;
  if
  (
  identifier(x)
  >
  const int(0)
  )
  {
  int
  identifier(foo)
  =
  const int(3)
  ;
  identifier(x)
  =
  identifier(x)
  +
  identifier(foo)
  ;
  }
  return
  identifier(x)
  ;
  }
  int
  identifier(foo)
  (
  void
  )
  {
  return
  const int(4)
  ;
  }
