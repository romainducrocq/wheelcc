-- Lexing ... OK
+
+
@@ Tokens @@
List[62]:
  long
  identifier(negative_one)
  =
  const long(1l)
  ;
  long
  identifier(zero)
  =
  const long(0l)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  identifier(negative_one)
  =
  -
  identifier(negative_one)
  ;
  if
  (
  const unsigned int(68719476736u)
  >=
  identifier(negative_one)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  -
  const int(2147483658)
  >=
  identifier(zero)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  !
  (
  const unsigned long(3ul)
  +
  const unsigned long(4294967293ul)
  )
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
