-- Lexing ... OK
+
+
@@ Tokens @@
List[65]:
  union
  identifier(u)
  {
  int
  identifier(arr)
  [
  const int(3)
  ]
  ;
  double
  identifier(d)
  ;
  }
  ;
  union
  identifier(u)
  identifier(get_union)
  (
  void
  )
  {
  union
  identifier(u)
  identifier(result)
  =
  {
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  }
  ,
  const double(4.0)
  }
  ;
  return
  identifier(result)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  *
  identifier(ptr)
  [
  const int(3)
  ]
  =
  &
  identifier(get_union)
  (
  )
  .
  identifier(arr)
  ;
  return
  const int(0)
  ;
  }
