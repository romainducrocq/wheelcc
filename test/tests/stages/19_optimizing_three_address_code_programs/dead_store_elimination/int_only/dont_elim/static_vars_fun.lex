-- Lexing ... OK
+
+
@@ Tokens @@
List[40]:
  int
  identifier(x)
  =
  const int(100)
  ;
  int
  identifier(get_x)
  (
  void
  )
  {
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
  identifier(x)
  =
  const int(5)
  ;
  int
  identifier(result)
  =
  identifier(get_x)
  (
  )
  ;
  identifier(x)
  =
  const int(10)
  ;
  return
  identifier(result)
  ;
  }
