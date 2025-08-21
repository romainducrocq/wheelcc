-- Lexing ... OK
+
+
@@ Tokens @@
List[76]:
  struct
  identifier(s)
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
  int
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(s)
  identifier(x)
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
  struct
  identifier(s)
  identifier(y)
  =
  {
  {
  const int(9)
  ,
  const int(8)
  ,
  const int(7)
  }
  ,
  const double(6.0)
  }
  ;
  int
  *
  identifier(arr)
  [
  const int(3)
  ]
  =
  &
  (
  (
  const int(1)
  ?
  identifier(x)
  :
  identifier(y)
  )
  .
  identifier(arr)
  )
  ;
  return
  const int(0)
  ;
  }
