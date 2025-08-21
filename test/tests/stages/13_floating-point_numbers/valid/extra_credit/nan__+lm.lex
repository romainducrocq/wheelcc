-- Lexing ... OK
+
+
@@ Tokens @@
List[293]:
  int
  identifier(double_isnan)
  (
  double
  identifier(d)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  static
  double
  identifier(zero)
  =
  const double(0.0)
  ;
  double
  identifier(nan)
  =
  const double(0.0)
  /
  identifier(zero)
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
  <
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
  if
  (
  !
  identifier(double_isnan)
  (
  identifier(nan)
  )
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  !
  identifier(double_isnan)
  (
  const int(4)
  *
  identifier(nan)
  )
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  !
  identifier(double_isnan)
  (
  const double(22e2)
  /
  identifier(nan)
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
  identifier(double_isnan)
  (
  -
  identifier(nan)
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
  identifier(nan)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(nan)
  )
  {
  }
  else
  {
  return
  const int(10)
  ;
  }
  int
  identifier(nan_is_nonzero)
  ;
  for
  (
  identifier(nan_is_nonzero)
  =
  const int(0)
  ;
  identifier(nan)
  ;
  )
  {
  identifier(nan_is_nonzero)
  =
  const int(1)
  ;
  break
  ;
  }
  if
  (
  !
  identifier(nan_is_nonzero)
  )
  {
  return
  const int(11)
  ;
  }
  identifier(nan_is_nonzero)
  =
  const int(0)
  ;
  while
  (
  identifier(nan)
  )
  {
  identifier(nan_is_nonzero)
  =
  const int(1)
  ;
  break
  ;
  }
  if
  (
  !
  identifier(nan_is_nonzero)
  )
  {
  return
  const int(12)
  ;
  }
  identifier(nan_is_nonzero)
  =
  -
  const int(1)
  ;
  do
  {
  identifier(nan_is_nonzero)
  =
  identifier(nan_is_nonzero)
  +
  const int(1)
  ;
  if
  (
  identifier(nan_is_nonzero)
  )
  {
  break
  ;
  }
  }
  while
  (
  identifier(nan)
  )
  ;
  if
  (
  !
  identifier(nan_is_nonzero)
  )
  {
  return
  const int(13)
  ;
  }
  identifier(nan_is_nonzero)
  =
  identifier(nan)
  ?
  const int(1)
  :
  const int(0)
  ;
  if
  (
  !
  identifier(nan_is_nonzero)
  )
  {
  return
  const int(14)
  ;
  }
  return
  const int(0)
  ;
  }
