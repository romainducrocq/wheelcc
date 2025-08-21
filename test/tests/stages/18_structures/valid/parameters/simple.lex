-- Lexing ... OK
+
+
@@ Tokens @@
List[75]:
  struct
  identifier(pair)
  {
  int
  identifier(x)
  ;
  double
  identifier(y)
  ;
  }
  ;
  double
  identifier(test_struct_param)
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
  const double(2.0)
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
  const double(2.0)
  }
  ;
  if
  (
  !
  identifier(test_struct_param)
  (
  identifier(x)
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
