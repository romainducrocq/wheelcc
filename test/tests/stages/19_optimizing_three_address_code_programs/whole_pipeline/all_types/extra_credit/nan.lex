-- Lexing ... OK
+
+
@@ Tokens @@
List[159]:
  int
  identifier(target)
  (
  void
  )
  {
  double
  identifier(nan)
  =
  const double(0.0)
  /
  const double(0.0)
  ;
  if
  (
  identifier(nan)
  <
  const double(0.0)
  ||
  identifier(nan)
  ==
  const double(0.0)
  ||
  identifier(nan)
  >
  const double(0.0)
  ||
  identifier(nan)
  <=
  const double(0.0)
  ||
  identifier(nan)
  >=
  const double(0.0)
  )
  return
  const int(1)
  ;
  if
  (
  const int(1)
  >
  identifier(nan)
  ||
  const int(1)
  ==
  identifier(nan)
  ||
  const int(1)
  >
  identifier(nan)
  ||
  const int(1)
  <=
  identifier(nan)
  ||
  const int(1)
  >=
  identifier(nan)
  )
  return
  const int(2)
  ;
  if
  (
  identifier(nan)
  ==
  identifier(nan)
  )
  return
  const int(3)
  ;
  if
  (
  !
  (
  identifier(nan)
  !=
  identifier(nan)
  )
  )
  {
  return
  const int(4)
  ;
  }
  identifier(nan)
  =
  identifier(nan)
  *
  const int(4)
  ;
  if
  (
  identifier(nan)
  ==
  identifier(nan)
  )
  {
  return
  const int(5)
  ;
  }
  identifier(nan)
  =
  const double(22e2)
  /
  identifier(nan)
  ;
  if
  (
  identifier(nan)
  ==
  identifier(nan)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  -
  identifier(nan)
  ==
  -
  identifier(nan)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  !
  identifier(nan)
  )
  {
  return
  const int(8)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  return
  identifier(target)
  (
  )
  ;
  }
