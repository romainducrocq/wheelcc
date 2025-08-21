-- Lexing ... OK
+
+
@@ Tokens @@
List[50]:
  struct
  identifier(pair)
  {
  int
  identifier(x)
  ;
  int
  identifier(y)
  ;
  }
  ;
  int
  identifier(validate_struct_param)
  (
  struct
  identifier(pair)
  identifier(p)
  )
  ;
  int
  identifier(validate_struct_param)
  (
  struct
  identifier(pair)
  identifier(p)
  )
  {
  if
  (
  identifier(p)
  .
  identifier(x)
  !=
  const int(1)
  ||
  identifier(p)
  .
  identifier(y)
  !=
  const int(2)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
