-- Lexing ... OK
+
+
@@ Tokens @@
List[99]:
  int
  identifier(strcmp)
  (
  char
  *
  identifier(s1)
  ,
  char
  *
  identifier(s2)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  char
  identifier(strings)
  [
  const int(2)
  ]
  [
  const int(13)
  ]
  =
  {
  string literal("abcdefghijkl")
  ,
  string literal("z")
  }
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(strings)
  [
  const int(0)
  ]
  ,
  string literal("abcdefghijkl")
  )
  )
  return
  const int(1)
  ;
  if
  (
  identifier(strings)
  [
  const int(1)
  ]
  [
  const int(0)
  ]
  !=
  const char('z')
  )
  return
  const int(2)
  ;
  for
  (
  int
  identifier(i)
  =
  const int(1)
  ;
  identifier(i)
  <
  const int(13)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  if
  (
  identifier(strings)
  [
  const int(1)
  ]
  [
  identifier(i)
  ]
  )
  return
  const int(3)
  ;
  }
  return
  const int(0)
  ;
  }
