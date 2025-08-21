-- Lexing ... OK
+
+
@@ Tokens @@
List[72]:
  double
  identifier(target)
  (
  int
  identifier(flag)
  ,
  int
  identifier(flag2)
  ,
  double
  identifier(y)
  )
  {
  double
  identifier(x)
  =
  identifier(y)
  ;
  if
  (
  identifier(flag)
  )
  {
  identifier(y)
  =
  identifier(x)
  ;
  }
  if
  (
  identifier(flag2)
  )
  {
  identifier(x)
  =
  identifier(y)
  ;
  }
  return
  identifier(x)
  +
  identifier(y)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(target)
  (
  const int(0)
  ,
  const int(1)
  ,
  const double(10.0)
  )
  !=
  const double(20.0)
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
