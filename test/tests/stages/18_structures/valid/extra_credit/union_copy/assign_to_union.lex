-- Lexing ... OK
+
+
@@ Tokens @@
List[182]:
  struct
  identifier(s)
  {
  int
  identifier(a)
  ;
  int
  identifier(b)
  ;
  }
  ;
  union
  identifier(u)
  {
  struct
  identifier(s)
  identifier(str)
  ;
  long
  identifier(l)
  ;
  double
  identifier(arr)
  [
  const int(3)
  ]
  ;
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  union
  identifier(u)
  identifier(x)
  =
  {
  {
  const int(1)
  ,
  const int(2)
  }
  }
  ;
  union
  identifier(u)
  identifier(y)
  =
  {
  {
  const int(0)
  ,
  const int(0)
  }
  }
  ;
  identifier(y)
  =
  identifier(x)
  ;
  if
  (
  identifier(y)
  .
  identifier(str)
  .
  identifier(a)
  !=
  const int(1)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(y)
  .
  identifier(str)
  .
  identifier(b)
  !=
  const int(2)
  )
  {
  return
  const int(2)
  ;
  }
  identifier(x)
  .
  identifier(arr)
  [
  const int(0)
  ]
  =
  -
  const double(20.)
  ;
  identifier(x)
  .
  identifier(arr)
  [
  const int(1)
  ]
  =
  -
  const double(30.)
  ;
  identifier(x)
  .
  identifier(arr)
  [
  const int(2)
  ]
  =
  -
  const double(40.)
  ;
  identifier(y)
  =
  identifier(x)
  ;
  if
  (
  identifier(y)
  .
  identifier(arr)
  [
  const int(0)
  ]
  !=
  -
  const double(20.)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(y)
  .
  identifier(arr)
  [
  const int(1)
  ]
  !=
  -
  const double(30.)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(y)
  .
  identifier(arr)
  [
  const int(2)
  ]
  !=
  -
  const double(40.)
  )
  {
  return
  const int(5)
  ;
  }
  return
  const int(0)
  ;
  }
