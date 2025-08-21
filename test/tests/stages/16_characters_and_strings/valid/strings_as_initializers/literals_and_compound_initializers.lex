-- Lexing ... OK
+
+
@@ Tokens @@
List[168]:
  signed
  char
  identifier(static_array)
  [
  const int(3)
  ]
  [
  const int(4)
  ]
  =
  {
  {
  const char('a')
  ,
  const char('b')
  ,
  const char('c')
  ,
  const char('d')
  }
  ,
  string literal("efgh")
  ,
  string literal("ijk")
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  unsigned
  char
  identifier(auto_array)
  [
  const int(2)
  ]
  [
  const int(3)
  ]
  =
  {
  string literal("lmn")
  ,
  {
  const char('o')
  ,
  const char('p')
  }
  }
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(3)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  for
  (
  int
  identifier(j)
  =
  const int(0)
  ;
  identifier(j)
  <
  const int(4)
  ;
  identifier(j)
  =
  identifier(j)
  +
  const int(1)
  )
  if
  (
  identifier(static_array)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  !=
  string literal("abcdefghijk")
  [
  identifier(i)
  *
  const int(4)
  +
  identifier(j)
  ]
  )
  return
  const int(1)
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(2)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  for
  (
  int
  identifier(j)
  =
  const int(0)
  ;
  identifier(j)
  <
  const int(3)
  ;
  identifier(j)
  =
  identifier(j)
  +
  const int(1)
  )
  if
  (
  identifier(auto_array)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  !=
  string literal("lmnop")
  [
  identifier(i)
  *
  const int(3)
  +
  identifier(j)
  ]
  )
  return
  const int(2)
  ;
  return
  const int(0)
  ;
  }
