-- Lexing ... OK
+
+
@@ Tokens @@
List[45]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  *
  identifier(null_ptr)
  =
  const int(0)
  ;
  if
  (
  &
  *
  identifier(null_ptr)
  !=
  const int(0)
  )
  return
  const int(1)
  ;
  int
  *
  *
  identifier(ptr_to_null)
  =
  &
  identifier(null_ptr)
  ;
  if
  (
  &
  *
  *
  identifier(ptr_to_null)
  )
  return
  const int(2)
  ;
  return
  const int(0)
  ;
  }
