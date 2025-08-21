-- Lexing ... OK
+
+
@@ Tokens @@
List[40]:
  int
  identifier(a)
  =
  const int(1)
  ;
  int
  identifier(b)
  =
  const int(2)
  ;
  int
  identifier(target)
  (
  void
  )
  {
  int
  identifier(unused)
  =
  identifier(a)
  *
  -
  identifier(b)
  ;
  return
  const int(5)
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
