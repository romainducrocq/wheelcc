-- Lexing ... OK
+
+
@@ Tokens @@
List[120]:
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(a)
  =
  const double(1.0)
  ;
  double
  identifier(b)
  =
  const double(1.)
  ;
  double
  identifier(c)
  =
  const double(1E0)
  ;
  double
  identifier(d)
  =
  const double(.01e+2)
  ;
  if
  (
  !
  (
  identifier(a)
  ==
  identifier(b)
  &&
  identifier(a)
  ==
  identifier(c)
  &&
  identifier(a)
  ==
  identifier(d)
  )
  )
  return
  const int(1)
  ;
  if
  (
  identifier(a)
  +
  identifier(b)
  +
  identifier(c)
  +
  identifier(d)
  !=
  const double(4.0)
  )
  return
  const int(2)
  ;
  double
  identifier(e)
  =
  const double(.125)
  ;
  double
  identifier(f)
  =
  const double(12.5e-2)
  ;
  double
  identifier(g)
  =
  const double(125.E-3)
  ;
  double
  identifier(h)
  =
  const double(1250000000e-10)
  ;
  if
  (
  !
  (
  identifier(e)
  ==
  identifier(f)
  &&
  identifier(e)
  ==
  identifier(g)
  &&
  identifier(e)
  ==
  identifier(h)
  )
  )
  return
  const int(3)
  ;
  if
  (
  identifier(e)
  +
  identifier(f)
  +
  identifier(g)
  +
  identifier(h)
  !=
  const double(0.5)
  )
  return
  const int(4)
  ;
  return
  const int(0)
  ;
  }
