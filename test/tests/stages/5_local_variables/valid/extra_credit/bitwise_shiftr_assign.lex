-- Lexing ... OK
+
+
@@ Tokens @@
List[26]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(var_to_shift)
  =
  const int(1234)
  ;
  int
  identifier(x)
  =
  const int(0)
  ;
  identifier(x)
  =
  identifier(var_to_shift)
  >>
  const int(4)
  ;
  return
  identifier(x)
  ;
  }
