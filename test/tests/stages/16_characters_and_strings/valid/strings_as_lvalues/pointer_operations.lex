-- Lexing ... OK
+
+
@@ Tokens @@
List[77]:
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  string literal("abcdefg")
  [
  const int(2)
  ]
  !=
  const char('c')
  )
  {
  return
  const int(1)
  ;
  }
  char
  *
  identifier(ptr)
  =
  string literal("This is a string!")
  +
  const int(10)
  ;
  if
  (
  *
  identifier(ptr)
  !=
  const char('s')
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(ptr)
  [
  const int(6)
  ]
  !=
  const char('!')
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(ptr)
  [
  const int(7)
  ]
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  !
  string literal("Not a null pointer!")
  )
  {
  return
  const int(5)
  ;
  }
  }
