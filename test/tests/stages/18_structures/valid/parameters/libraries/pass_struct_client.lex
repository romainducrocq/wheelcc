-- Lexing ... OK
+
+
@@ Tokens @@
List[52]:
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
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(pair)
  identifier(arg)
  =
  {
  const int(1)
  ,
  const int(2)
  }
  ;
  if
  (
  !
  identifier(validate_struct_param)
  (
  identifier(arg)
  )
  )
  {
  return
  const int(1)
  ;
  }
  return
  const int(0)
  ;
  }
