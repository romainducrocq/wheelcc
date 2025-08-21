-- Lexing ... OK
+
+
@@ Tokens @@
List[113]:
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
  identifier(puts)
  (
  char
  *
  identifier(s)
  )
  ;
  unsigned
  long
  identifier(strlen)
  (
  char
  *
  identifier(s)
  )
  ;
  int
  identifier(atoi)
  (
  char
  *
  identifier(s)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(strcmp)
  (
  string literal("abc")
  ,
  string literal("abc")
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
  string literal("ab")
  ,
  string literal("xy")
  )
  >=
  const int(0)
  )
  {
  return
  const int(2)
  ;
  }
  identifier(puts)
  (
  string literal("Hello, World!")
  )
  ;
  if
  (
  identifier(strlen)
  (
  string literal("")
  )
  )
  {
  return
  const int(3)
  ;
  }
  int
  identifier(i)
  =
  identifier(atoi)
  (
  string literal("10")
  )
  ;
  if
  (
  identifier(i)
  !=
  const int(10)
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
