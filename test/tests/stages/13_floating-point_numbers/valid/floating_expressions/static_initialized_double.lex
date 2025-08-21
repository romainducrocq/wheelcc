-- Lexing ... OK
+
+
@@ Tokens @@
List[91]:
  double
  identifier(return_static_variable)
  (
  void
  )
  {
  static
  double
  identifier(d)
  =
  const double(0.5)
  ;
  double
  identifier(ret)
  =
  identifier(d)
  ;
  identifier(d)
  =
  identifier(d)
  +
  const double(1.0)
  ;
  return
  identifier(ret)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(d1)
  =
  identifier(return_static_variable)
  (
  )
  ;
  double
  identifier(d2)
  =
  identifier(return_static_variable)
  (
  )
  ;
  double
  identifier(d3)
  =
  identifier(return_static_variable)
  (
  )
  ;
  if
  (
  identifier(d1)
  !=
  const double(0.5)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(d2)
  !=
  const double(1.5)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(d3)
  !=
  const double(2.5)
  )
  {
  return
  const int(3)
  ;
  }
  return
  const int(0)
  ;
  }
