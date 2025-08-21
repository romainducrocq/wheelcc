-- Lexing ... OK
+
+
@@ Tokens @@
List[53]:
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
  const int(2)
  ;
  identifier(i)
  >>=
  const unsigned int(3u)
  ;
  if
  (
  identifier(i)
  !=
  -
  const int(1)
  )
  {
  return
  const int(1)
  ;
  }
  unsigned
  long
  identifier(ul)
  =
  const unsigned long(18446744073709551615UL)
  ;
  identifier(ul)
  <<=
  const int(44)
  ;
  if
  (
  identifier(ul)
  !=
  const unsigned long(18446726481523507200ul)
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
