-- Lexing ... OK
+
+
@@ Tokens @@
List[20]:
  double
  *
  identifier(d_ptr)
  ;
  int
  identifier(update_thru_ptr)
  (
  double
  identifier(new_val)
  )
  {
  *
  identifier(d_ptr)
  =
  identifier(new_val)
  ;
  return
  const int(0)
  ;
  }
