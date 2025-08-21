-- Lexing ... OK
+
+
@@ Tokens @@
List[156]:
  int
  identifier(idx)
  =
  const int(3)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(arr)
  [
  const int(5)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  ,
  const int(4)
  ,
  const int(5)
  }
  ;
  int
  *
  identifier(ptr)
  =
  identifier(arr)
  +
  const int(1)
  ;
  int
  identifier(result)
  =
  ++
  identifier(ptr)
  --
  [
  identifier(idx)
  ]
  ;
  if
  (
  identifier(result)
  !=
  const int(6)
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
  const int(1)
  )
  {
  return
  const int(2)
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
  const int(3)
  ;
  }
  if
  (
  *
  identifier(ptr)
  ++
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
  *
  identifier(ptr)
  !=
  const int(2)
  )
  {
  return
  const int(5)
  ;
  }
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(4)
  ;
  identifier(i)
  ++
  )
  {
  if
  (
  identifier(arr)
  [
  identifier(i)
  ]
  !=
  identifier(i)
  +
  const int(1)
  )
  {
  return
  const int(6)
  ;
  }
  }
  if
  (
  identifier(arr)
  [
  const int(4)
  ]
  !=
  const int(6)
  )
  {
  return
  const int(7)
  ;
  }
  return
  const int(0)
  ;
  }
