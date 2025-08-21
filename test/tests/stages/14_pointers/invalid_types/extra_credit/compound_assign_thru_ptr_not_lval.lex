-- Lexing ... OK
+
+
@@ Tokens @@
List[34]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(i)
  =
  const int(100)
  ;
  int
  *
  identifier(ptr)
  =
  &
  identifier(i)
  ;
  int
  *
  identifier(ptr2)
  =
  &
  (
  *
  identifier(ptr)
  -=
  const int(10)
  )
  ;
  return
  const int(0)
  ;
  }
