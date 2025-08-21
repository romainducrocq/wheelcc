-- Lexing ... OK
+
+
@@ Tokens @@
List[35]:
  unsigned
  long
  identifier(ul)
  =
  const unsigned long(10000ul)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(i)
  =
  -
  const int(50)
  ;
  double
  identifier(d)
  =
  (
  identifier(ul)
  +
  identifier(i)
  )
  *
  const double(3.125)
  ;
  return
  identifier(d)
  ==
  const double(31093.75)
  ;
  }
