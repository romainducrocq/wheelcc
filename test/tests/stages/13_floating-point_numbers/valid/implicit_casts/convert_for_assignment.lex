-- Lexing ... OK
+
+
@@ Tokens @@
List[121]:
  int
  identifier(check_args)
  (
  long
  identifier(l)
  ,
  double
  identifier(d)
  )
  {
  return
  identifier(l)
  ==
  const int(2)
  &&
  identifier(d)
  ==
  -
  const double(6.0)
  ;
  }
  double
  identifier(return_double)
  (
  void
  )
  {
  return
  const unsigned long(18446744073709551586ul)
  ;
  }
  int
  identifier(check_assignment)
  (
  double
  identifier(arg)
  )
  {
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  =
  identifier(arg)
  ;
  return
  identifier(i)
  ==
  const int(4)
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
  identifier(check_args)
  (
  const double(2.4)
  ,
  -
  const int(6)
  )
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(return_double)
  (
  )
  !=
  const double(18446744073709551616.0)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  !
  identifier(check_assignment)
  (
  const double(4.9)
  )
  )
  {
  return
  const int(3)
  ;
  }
  double
  identifier(d)
  =
  const unsigned long(18446744073709551586ul)
  ;
  if
  (
  identifier(d)
  !=
  const double(18446744073709551616.)
  )
  {
  return
  const int(4)
  ;
  }
  return
  const int(0)
  ;
  }
