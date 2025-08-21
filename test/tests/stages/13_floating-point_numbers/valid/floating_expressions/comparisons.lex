-- Lexing ... OK
+
+
@@ Tokens @@
List[197]:
  double
  identifier(fifty_fiveE5)
  =
  const double(55e5)
  ;
  double
  identifier(fifty_fourE4)
  =
  const double(54e4)
  ;
  double
  identifier(tiny)
  =
  const double(.00004)
  ;
  double
  identifier(four)
  =
  const double(4.)
  ;
  double
  identifier(point_one)
  =
  const double(0.1)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(fifty_fiveE5)
  <
  identifier(fifty_fourE4)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(four)
  >
  const double(4.0)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(tiny)
  <=
  const double(0.0)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(fifty_fourE4)
  >=
  identifier(fifty_fiveE5)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(tiny)
  ==
  const double(0.0)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(point_one)
  !=
  identifier(point_one)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  !
  (
  identifier(tiny)
  >
  const double(00.000005)
  )
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  !
  (
  -
  const double(.00004)
  <
  identifier(four)
  )
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  !
  (
  identifier(tiny)
  <=
  identifier(tiny)
  )
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  !
  (
  identifier(fifty_fiveE5)
  >=
  identifier(fifty_fiveE5)
  )
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  !
  (
  const double(0.1)
  ==
  identifier(point_one)
  )
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  !
  (
  identifier(tiny)
  !=
  const double(.00003)
  )
  )
  {
  return
  const int(12)
  ;
  }
  if
  (
  const double(0.00003)
  <
  const double(0.000000000003)
  )
  {
  return
  const int(13)
  ;
  }
  return
  const int(0)
  ;
  }
