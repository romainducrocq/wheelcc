-- Lexing ... OK
+
+
@@ Tokens @@
List[47]:
  int
  identifier(main)
  (
  void
  )
  {
  long
  identifier(x)
  =
  const int(10)
  ;
  long
  *
  identifier(ptr)
  =
  &
  identifier(x)
  +
  const int(1)
  ;
  long
  (
  *
  identifier(array_ptr)
  )
  [
  const int(10)
  ]
  =
  (
  long
  (
  *
  )
  [
  const int(10)
  ]
  )
  &
  identifier(x)
  ;
  return
  identifier(array_ptr)
  <
  identifier(ptr)
  ;
  }
