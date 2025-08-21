-- Lexing ... OK
+
+
@@ Tokens @@
List[40]:
  int
  identifier(x)
  ;
  extern
  int
  identifier(x)
  ;
  int
  identifier(x)
  ;
  int
  identifier(update_x)
  (
  int
  identifier(new_val)
  )
  {
  identifier(x)
  =
  identifier(new_val)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(read_x)
  (
  void
  )
  {
  return
  identifier(x)
  ;
  }
  int
  identifier(x)
  =
  const int(3)
  ;
