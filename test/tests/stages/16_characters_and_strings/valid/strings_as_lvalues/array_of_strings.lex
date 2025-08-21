-- Lexing ... OK
+
+
@@ Tokens @@
List[100]:
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
  *
  identifier(strings)
  [
  const int(4)
  ]
  =
  {
  string literal("yes")
  ,
  string literal("no")
  ,
  string literal("maybe")
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
  string literal("yes")
  )
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(strcmp)
  (
  identifier(strings)
  [
  const int(1)
  ]
  ,
  string literal("no")
  )
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(strcmp)
  (
  identifier(strings)
  [
  const int(2)
  ]
  ,
  string literal("maybe")
  )
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(strings)
  [
  const int(3)
  ]
  )
  {
  return
  const int(4)
  ;
  }
  return
  const int(0)
  ;
  }
