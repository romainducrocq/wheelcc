-- Lexing ... OK
+
+
@@ Tokens @@
List[104]:
  int
  identifier(target)
  (
  void
  )
  {
  double
  identifier(d1)
  =
  const double(9007199254740991.0)
  ;
  double
  identifier(d2)
  =
  identifier(d1)
  ++
  ;
  double
  identifier(d3)
  =
  ++
  identifier(d1)
  ;
  double
  identifier(e1)
  =
  const double(10.0)
  ;
  double
  identifier(e2)
  =
  --
  identifier(e1)
  ;
  double
  identifier(e3)
  =
  identifier(e1)
  --
  ;
  if
  (
  !
  (
  identifier(d1)
  ==
  const double(9007199254740992.0)
  &&
  identifier(d2)
  ==
  const double(9007199254740991.0)
  &&
  identifier(d1)
  >
  identifier(d2)
  &&
  identifier(d1)
  ==
  identifier(d3)
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
  (
  identifier(e1)
  ==
  const double(8.)
  &&
  identifier(e2)
  ==
  const double(9.)
  &&
  identifier(e3)
  ==
  const double(9.)
  )
  )
  {
  return
  const int(2)
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
