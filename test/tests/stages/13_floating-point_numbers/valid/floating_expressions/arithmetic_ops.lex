-- Lexing ... OK
+
+
@@ Tokens @@
List[228]:
  double
  identifier(point_one)
  =
  const double(0.1)
  ;
  double
  identifier(point_two)
  =
  const double(0.2)
  ;
  double
  identifier(point_three)
  =
  const double(0.3)
  ;
  double
  identifier(two)
  =
  const double(2.0)
  ;
  double
  identifier(three)
  =
  const double(3.0)
  ;
  double
  identifier(four)
  =
  const double(4.0)
  ;
  double
  identifier(twelveE30)
  =
  const double(12e30)
  ;
  int
  identifier(addition)
  (
  void
  )
  {
  return
  (
  identifier(point_one)
  +
  identifier(point_two)
  ==
  const double(0.30000000000000004)
  )
  ;
  }
  int
  identifier(subtraction)
  (
  void
  )
  {
  return
  (
  identifier(four)
  -
  const double(1.0)
  ==
  const double(3.0)
  )
  ;
  }
  int
  identifier(multiplication)
  (
  void
  )
  {
  return
  (
  const double(0.01)
  *
  identifier(point_three)
  ==
  const double(0.003)
  )
  ;
  }
  int
  identifier(division)
  (
  void
  )
  {
  return
  (
  const double(7.0)
  /
  identifier(two)
  ==
  const double(3.5)
  )
  ;
  }
  int
  identifier(negation)
  (
  void
  )
  {
  double
  identifier(neg)
  =
  -
  identifier(twelveE30)
  ;
  return
  !
  (
  const double(12e30)
  +
  identifier(neg)
  )
  ;
  }
  int
  identifier(complex_expression)
  (
  void
  )
  {
  double
  identifier(complex_expression)
  =
  (
  identifier(two)
  +
  identifier(three)
  )
  -
  const double(127.5)
  *
  identifier(four)
  ;
  return
  identifier(complex_expression)
  ==
  -
  const double(505.0)
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
  !
  identifier(addition)
  (
  )
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  !
  identifier(subtraction)
  (
  )
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  !
  identifier(multiplication)
  (
  )
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  !
  identifier(division)
  (
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
  identifier(negation)
  (
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
  identifier(complex_expression)
  (
  )
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
