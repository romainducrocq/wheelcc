-- Lexing ... OK
+
+
@@ Tokens @@
List[90]:
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
  identifier(multi_string)
  [
  const int(6)
  ]
  =
  string literal("yes")
  string literal("no")
  ;
  char
  identifier(nested_multi_string)
  [
  const int(2)
  ]
  [
  const int(3)
  ]
  =
  {
  string literal("a")
  string literal("b")
  ,
  string literal("c")
  string literal("d")
  }
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(multi_string)
  ,
  string literal("yesno")
  )
  )
  return
  const int(1)
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(nested_multi_string)
  [
  const int(0)
  ]
  ,
  string literal("ab")
  )
  )
  return
  const int(2)
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(nested_multi_string)
  [
  const int(1)
  ]
  ,
  string literal("cd")
  )
  )
  return
  const int(3)
  ;
  return
  const int(0)
  ;
  }
