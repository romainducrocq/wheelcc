-- Lexing ... OK
+
+
@@ Tokens @@
List[144]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(arr)
  [
  const int(4)
  ]
  =
  {
  -
  const int(1)
  ,
  -
  const int(2)
  ,
  -
  const int(3)
  ,
  -
  const int(4)
  }
  ;
  int
  *
  identifier(ptr)
  =
  identifier(arr)
  ;
  int
  identifier(idx)
  =
  const int(2)
  ;
  if
  (
  (
  identifier(ptr)
  ++
  [
  identifier(idx)
  ++
  ]
  *=
  const int(3)
  )
  !=
  -
  const int(9)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  *
  identifier(ptr)
  !=
  -
  const int(2)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(idx)
  !=
  const int(3)
  )
  {
  return
  const int(3)
  ;
  }
  identifier(idx)
  --
  ;
  if
  (
  (
  --
  identifier(ptr)
  )
  [
  const int(3)
  ]
  +=
  const int(4)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(arr)
  [
  const int(0)
  ]
  !=
  -
  const int(1)
  ||
  identifier(arr)
  [
  const int(1)
  ]
  !=
  -
  const int(2)
  ||
  identifier(arr)
  [
  const int(2)
  ]
  !=
  -
  const int(9)
  ||
  identifier(arr)
  [
  const int(3)
  ]
  !=
  const int(0)
  )
  {
  return
  const int(5)
  ;
  }
  return
  const int(0)
  ;
  }
