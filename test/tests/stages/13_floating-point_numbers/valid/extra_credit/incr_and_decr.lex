-- Lexing ... OK
+
+
@@ Tokens @@
List[155]:
  int
  identifier(main)
  (
  void
  )
  {
  static
  double
  identifier(d)
  =
  const double(0.75)
  ;
  if
  (
  identifier(d)
  ++
  !=
  const double(0.75)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(d)
  !=
  const double(1.75)
  )
  {
  return
  const int(2)
  ;
  }
  identifier(d)
  =
  -
  const double(100.2)
  ;
  if
  (
  ++
  identifier(d)
  !=
  -
  const double(99.2)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(d)
  !=
  -
  const double(99.2)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(d)
  --
  !=
  -
  const double(99.2)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(d)
  !=
  -
  const double(100.2)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  --
  identifier(d)
  !=
  -
  const double(101.2)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(d)
  !=
  -
  const double(101.2)
  )
  {
  return
  const int(8)
  ;
  }
  identifier(d)
  =
  const double(0.000000000000000000001)
  ;
  identifier(d)
  ++
  ;
  if
  (
  identifier(d)
  !=
  const double(1.0)
  )
  {
  return
  const int(9)
  ;
  }
  identifier(d)
  =
  const double(10e20)
  ;
  identifier(d)
  --
  ;
  if
  (
  identifier(d)
  !=
  const double(10e20)
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
