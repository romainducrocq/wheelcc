-- Lexing ... OK
+
+
@@ Tokens @@
List[40]:
  int
  identifier(glob)
  =
  const int(0)
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
  identifier(glob)
  ;
  identifier(x)
  *=
  const int(20)
  ;
  identifier(x)
  =
  const int(10)
  ;
  return
  identifier(x)
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
