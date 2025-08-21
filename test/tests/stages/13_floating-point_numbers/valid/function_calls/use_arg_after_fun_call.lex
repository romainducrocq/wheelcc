-- Lexing ... OK
+
+
@@ Tokens @@
List[48]:
  double
  identifier(fun)
  (
  double
  identifier(x)
  )
  {
  if
  (
  identifier(x)
  >
  const int(2)
  )
  return
  identifier(x)
  ;
  else
  {
  double
  identifier(ret)
  =
  identifier(fun)
  (
  identifier(x)
  +
  const int(2)
  )
  ;
  return
  identifier(ret)
  +
  identifier(x)
  ;
  }
  }
  int
  identifier(main)
  (
  void
  )
  {
  return
  identifier(fun)
  (
  const double(1.0)
  )
  ;
  }
