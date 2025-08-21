-- Lexing ... OK
+
+
@@ Tokens @@
List[173]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(i)
  =
  const int(0)
  ;
  long
  identifier(l)
  =
  const int(0)
  ;
  static
  char
  identifier(arr)
  [
  const int(3)
  ]
  =
  {
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  }
  ;
  char
  *
  identifier(ptr)
  =
  identifier(arr)
  ;
  if
  (
  sizeof
  (
  identifier(i)
  ++
  )
  !=
  const int(4)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  sizeof
  (
  identifier(arr)
  [
  const int(0)
  ]
  --
  )
  !=
  const int(1)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  sizeof
  (
  ++
  identifier(l)
  )
  !=
  const int(8)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  sizeof
  (
  --
  identifier(arr)
  [
  const int(1)
  ]
  )
  !=
  const int(1)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  sizeof
  (
  identifier(ptr)
  --
  )
  !=
  const int(8)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(i)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(l)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(arr)
  [
  const int(0)
  ]
  ||
  identifier(arr)
  [
  const int(1)
  ]
  ||
  identifier(arr)
  [
  const int(2)
  ]
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(ptr)
  !=
  identifier(arr)
  )
  {
  return
  const int(9)
  ;
  }
  return
  const int(0)
  ;
  }
