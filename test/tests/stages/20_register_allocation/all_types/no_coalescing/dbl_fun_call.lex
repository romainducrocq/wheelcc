-- Lexing ... OK
+
+
@@ Tokens @@
List[39]:
  double
  identifier(glob)
  =
  const double(3.0)
  ;
  double
  identifier(callee)
  (
  void
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(d)
  =
  identifier(glob)
  ;
  double
  identifier(x)
  =
  identifier(callee)
  (
  )
  ;
  return
  (
  identifier(d)
  +
  identifier(x)
  ==
  const double(13.0)
  )
  ;
  }
