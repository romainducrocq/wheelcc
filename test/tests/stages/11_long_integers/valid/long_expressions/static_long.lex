-- Lexing ... OK
+
+
@@ Tokens @@
List[39]:
  static
  long
  identifier(foo)
  =
  const long(4294967290l)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(foo)
  +
  const long(5l)
  ==
  const long(4294967295l)
  )
  {
  identifier(foo)
  =
  const long(1152921504606846988l)
  ;
  if
  (
  identifier(foo)
  ==
  const long(1152921504606846988l)
  )
  return
  const int(1)
  ;
  }
  return
  const int(0)
  ;
  }
