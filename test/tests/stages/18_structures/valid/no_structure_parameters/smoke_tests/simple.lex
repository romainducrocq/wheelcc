-- Lexing ... OK
+
+
@@ Tokens @@
List[77]:
  struct
  identifier(pair)
  {
  int
  identifier(a)
  ;
  int
  identifier(b)
  ;
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(pair)
  identifier(x)
  =
  {
  const int(1)
  ,
  const int(2)
  }
  ;
  if
  (
  identifier(x)
  .
  identifier(a)
  !=
  const int(1)
  ||
  identifier(x)
  .
  identifier(b)
  !=
  const int(2)
  )
  {
  return
  const int(1)
  ;
  }
  struct
  identifier(pair)
  *
  identifier(x_ptr)
  =
  &
  identifier(x)
  ;
  if
  (
  identifier(x_ptr)
  ->
  identifier(a)
  !=
  const int(1)
  ||
  identifier(x_ptr)
  ->
  identifier(b)
  !=
  const int(2)
  )
  {
  return
  const int(2)
  ;
  }
  return
  const int(0)
  ;
  }
