-- Lexing ... OK
+
+
@@ Tokens @@
List[33]:
  int
  identifier(i)
  ;
  long
  *
  identifier(return_long_pointer)
  (
  void
  )
  {
  return
  &
  identifier(i)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  long
  *
  identifier(l)
  =
  identifier(return_long_pointer)
  (
  )
  ;
  return
  const int(0)
  ;
  }
