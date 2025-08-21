-- Lexing ... OK
+
+
@@ Tokens @@
List[32]:
  static
  long
  *
  identifier(long_ptr)
  ;
  long
  *
  identifier(get_pointer)
  (
  void
  )
  {
  return
  identifier(long_ptr)
  ;
  }
  int
  identifier(set_pointer)
  (
  long
  *
  identifier(new_ptr)
  )
  {
  identifier(long_ptr)
  =
  identifier(new_ptr)
  ;
  return
  const int(0)
  ;
  }
