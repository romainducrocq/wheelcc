-- Lexing ... OK
+
+
@@ Tokens @@
List[42]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(a)
  =
  const int(0)
  ;
  int
  identifier(b)
  =
  const int(0)
  ;
  do
  identifier(do_body)
  :
  identifier(a)
  =
  identifier(a)
  +
  const int(1)
  ;
  identifier(b)
  =
  identifier(b)
  -
  const int(1)
  ;
  while
  (
  identifier(a)
  <
  const int(10)
  )
  ;
  return
  const int(0)
  ;
  }
