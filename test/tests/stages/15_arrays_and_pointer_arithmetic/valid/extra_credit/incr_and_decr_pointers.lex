-- Lexing ... OK
+
+
@@ Tokens @@
List[158]:
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(x)
  [
  const int(3)
  ]
  =
  {
  const double(0.0)
  ,
  const double(1.0)
  ,
  const double(2.0)
  }
  ;
  double
  *
  identifier(ptr)
  =
  identifier(x)
  ;
  if
  (
  ++
  identifier(ptr)
  !=
  identifier(x)
  +
  const int(1)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  *
  identifier(ptr)
  !=
  const double(1.0)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(ptr)
  ++
  !=
  identifier(x)
  +
  const int(1)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(ptr)
  !=
  identifier(x)
  +
  const int(2)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  *
  identifier(ptr)
  !=
  const double(2.0)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  --
  identifier(ptr)
  !=
  identifier(x)
  +
  const int(1)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  *
  identifier(ptr)
  !=
  const double(1.0)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(ptr)
  --
  !=
  identifier(x)
  +
  const int(1)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  *
  identifier(ptr)
  !=
  const double(0.0)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(ptr)
  !=
  identifier(x)
  )
  {
  return
  const int(10)
  ;
  }
  return
  const int(0)
  ;
  }
