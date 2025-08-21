-- Lexing ... OK
+
+
@@ Tokens @@
List[42]:
  int
  static
  identifier(foo)
  (
  void
  )
  {
  return
  const int(3)
  ;
  }
  int
  static
  identifier(bar)
  =
  const int(4)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  int
  extern
  identifier(foo)
  (
  void
  )
  ;
  int
  extern
  identifier(bar)
  ;
  return
  identifier(foo)
  (
  )
  +
  identifier(bar)
  ;
  }
