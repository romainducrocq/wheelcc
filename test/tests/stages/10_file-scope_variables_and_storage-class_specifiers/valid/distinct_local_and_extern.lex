-- Lexing ... OK
+
+
@@ Tokens @@
List[53]:
  int
  identifier(a)
  =
  const int(5)
  ;
  int
  identifier(return_a)
  (
  void
  )
  {
  return
  identifier(a)
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
  const int(3)
  ;
  {
  extern
  int
  identifier(a)
  ;
  if
  (
  identifier(a)
  !=
  const int(5)
  )
  return
  const int(1)
  ;
  identifier(a)
  =
  const int(4)
  ;
  }
  return
  identifier(a)
  +
  identifier(return_a)
  (
  )
  ;
  }
