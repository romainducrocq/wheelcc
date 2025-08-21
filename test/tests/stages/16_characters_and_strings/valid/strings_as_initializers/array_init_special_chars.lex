-- Lexing ... OK
+
+
@@ Tokens @@
List[102]:
  int
  identifier(main)
  (
  void
  )
  {
  char
  identifier(special)
  [
  const int(6)
  ]
  =
  string literal("\a\b\n	")
  ;
  if
  (
  identifier(special)
  [
  const int(0)
  ]
  !=
  const char('\a')
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(special)
  [
  const int(1)
  ]
  !=
  const char('\b')
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(special)
  [
  const int(2)
  ]
  !=
  const char('\n')
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(special)
  [
  const int(3)
  ]
  !=
  const char('\v')
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(special)
  [
  const int(4)
  ]
  !=
  const char('\f')
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(special)
  [
  const int(5)
  ]
  !=
  const char('\t')
  )
  {
  return
  const int(6)
  ;
  }
  return
  const int(0)
  ;
  }
