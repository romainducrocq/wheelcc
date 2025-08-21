-- Lexing ... OK
+
+
@@ Tokens @@
List[113]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(10)
  ;
  int
  *
  identifier(ptr)
  =
  &
  identifier(x)
  ;
  *
  identifier(ptr)
  +=
  const int(5)
  ;
  if
  (
  identifier(x)
  !=
  const int(15)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  (
  *
  identifier(ptr)
  -=
  const int(12)
  )
  !=
  const int(3)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(x)
  !=
  const int(3)
  )
  {
  return
  const int(3)
  ;
  }
  *
  identifier(ptr)
  *=
  const int(6)
  ;
  if
  (
  identifier(x)
  !=
  const int(18)
  )
  {
  return
  const int(4)
  ;
  }
  *
  identifier(ptr)
  /=
  const int(9)
  ;
  if
  (
  identifier(x)
  !=
  const int(2)
  )
  {
  return
  const int(5)
  ;
  }
  *
  identifier(ptr)
  %=
  const int(3)
  ;
  if
  (
  identifier(x)
  !=
  const int(2)
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
