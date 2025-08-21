-- Lexing ... OK
+
+
@@ Tokens @@
List[93]:
  union
  identifier(u)
  {
  long
  identifier(l)
  ;
  int
  identifier(i)
  ;
  char
  identifier(c)
  ;
  }
  ;
  int
  identifier(choose_union)
  (
  int
  identifier(flag)
  )
  {
  union
  identifier(u)
  identifier(one)
  ;
  union
  identifier(u)
  identifier(two)
  ;
  identifier(one)
  .
  identifier(l)
  =
  -
  const int(1)
  ;
  identifier(two)
  .
  identifier(i)
  =
  const int(100)
  ;
  return
  (
  identifier(flag)
  ?
  identifier(one)
  :
  identifier(two)
  )
  .
  identifier(c)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(choose_union)
  (
  const int(1)
  )
  !=
  -
  const int(1)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(choose_union)
  (
  const int(0)
  )
  !=
  const int(100)
  )
  {
  return
  const int(2)
  ;
  }
  return
  const int(0)
  ;
  }
