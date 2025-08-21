-- Lexing ... OK
+
+
@@ Tokens @@
List[42]:
  int
  identifier(bar)
  (
  void
  )
  {
  return
  const int(9)
  ;
  }
  int
  identifier(foo)
  (
  void
  )
  {
  return
  const int(2)
  *
  identifier(bar)
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
  return
  identifier(foo)
  (
  )
  +
  identifier(bar)
  (
  )
  /
  const int(3)
  ;
  }
