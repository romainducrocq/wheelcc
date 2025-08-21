-- Lexing ... OK
+
+
@@ Tokens @@
List[126]:
  int
  identifier(arr)
  [
  const int(3)
  ]
  =
  {
  const int(1)
  ,
  const int(1)
  ,
  const int(1)
  }
  ;
  int
  (
  *
  identifier(foo)
  (
  int
  identifier(x)
  ,
  int
  identifier(y)
  )
  )
  [
  const int(3)
  ]
  {
  identifier(arr)
  [
  const int(1)
  ]
  =
  identifier(x)
  ;
  identifier(arr)
  [
  const int(2)
  ]
  =
  identifier(y)
  ;
  return
  &
  identifier(arr)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  (
  *
  identifier(arr)
  )
  [
  const int(3)
  ]
  =
  identifier(foo)
  (
  const int(2)
  ,
  const int(3)
  )
  ;
  if
  (
  identifier(arr)
  [
  const int(0)
  ]
  [
  const int(0)
  ]
  !=
  const int(1)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(arr)
  [
  const int(0)
  ]
  [
  const int(1)
  ]
  !=
  const int(2)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(arr)
  [
  const int(0)
  ]
  [
  const int(2)
  ]
  !=
  const int(3)
  )
  {
  return
  const int(3)
  ;
  }
  return
  const int(0)
  ;
  }
