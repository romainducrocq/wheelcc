-- Lexing ... OK
+
+
@@ Tokens @@
List[72]:
  int
  *
  identifier(return_pointer)
  (
  int
  *
  identifier(in)
  )
  {
  return
  identifier(in)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(10)
  ;
  int
  *
  identifier(x_ptr)
  =
  identifier(return_pointer)
  (
  &
  identifier(x)
  )
  ;
  if
  (
  *
  identifier(x_ptr)
  !=
  const int(10)
  )
  return
  const int(1)
  ;
  identifier(x)
  =
  const int(100)
  ;
  if
  (
  *
  identifier(x_ptr)
  !=
  const int(100)
  )
  return
  const int(2)
  ;
  if
  (
  identifier(x_ptr)
  !=
  &
  identifier(x)
  )
  return
  const int(3)
  ;
  return
  const int(0)
  ;
  }
