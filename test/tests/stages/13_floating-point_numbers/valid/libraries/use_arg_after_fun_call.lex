-- Lexing ... OK
+
+
@@ Tokens @@
List[35]:
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
