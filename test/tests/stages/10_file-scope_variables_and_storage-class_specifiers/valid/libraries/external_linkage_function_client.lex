-- Lexing ... OK
+
+
@@ Tokens @@
List[131]:
  int
  identifier(add_one_and_two)
  (
  void
  )
  {
  extern
  int
  identifier(sum)
  (
  int
  identifier(a)
  ,
  int
  identifier(b)
  )
  ;
  int
  identifier(sum)
  (
  int
  identifier(a)
  ,
  int
  identifier(b)
  )
  ;
  return
  identifier(sum)
  (
  const int(1)
  ,
  const int(2)
  )
  ;
  }
  extern
  int
  identifier(sum)
  (
  int
  identifier(x)
  ,
  int
  identifier(y)
  )
  ;
  int
  identifier(sum)
  (
  int
  identifier(x)
  ,
  int
  identifier(y)
  )
  ;
  int
  identifier(add_three_and_four)
  (
  void
  )
  {
  int
  identifier(sum)
  =
  const int(3)
  ;
  if
  (
  identifier(sum)
  >
  const int(2)
  )
  {
  extern
  int
  identifier(sum)
  (
  int
  identifier(one)
  ,
  int
  identifier(two)
  )
  ;
  return
  identifier(sum)
  (
  const int(3)
  ,
  const int(4)
  )
  ;
  }
  return
  const int(1)
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
  identifier(add_three_and_four)
  (
  )
  !=
  const int(7)
  )
  return
  const int(1)
  ;
  if
  (
  identifier(add_one_and_two)
  (
  )
  !=
  const int(3)
  )
  return
  const int(1)
  ;
  return
  const int(0)
  ;
  }
