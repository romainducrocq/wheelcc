-- Lexing ... OK
+
+
@@ Tokens @@
List[68]:
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
  (
  const int(1)
  ?
  identifier(x)
  :
  identifier(y)
  )
  .
  identifier(d)
  =
  const double(0.0)
  ;
  return
  const int(0)
  ;
  }
