-- Lexing ... OK
+
+
@@ Tokens @@
List[187]:
  int
  identifier(i)
  =
  const int(2)
  ;
  int
  identifier(j)
  =
  const int(1)
  ;
  int
  identifier(k)
  =
  const int(0)
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
  const int(3)
  ]
  [
  const int(2)
  ]
  [
  const int(2)
  ]
  =
  {
  {
  {
  const int(1)
  ,
  const int(2)
  }
  ,
  {
  const int(3)
  ,
  const int(4)
  }
  }
  ,
  {
  {
  const int(5)
  ,
  const int(6)
  }
  ,
  {
  const int(7)
  ,
  const int(8)
  }
  }
  ,
  {
  {
  const int(9)
  ,
  const int(10)
  }
  ,
  {
  const int(11)
  ,
  const int(12)
  }
  }
  }
  ;
  if
  (
  identifier(arr)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  [
  identifier(k)
  ]
  ++
  !=
  const int(11)
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
  identifier(i)
  ]
  [
  identifier(j)
  ]
  [
  identifier(k)
  ]
  !=
  const int(12)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  ++
  identifier(arr)
  [
  --
  identifier(i)
  ]
  [
  identifier(j)
  --
  ]
  [
  ++
  identifier(k)
  ]
  !=
  const int(9)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(arr)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  [
  identifier(k)
  ]
  !=
  const int(6)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  --
  identifier(arr)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  [
  identifier(k)
  ]
  !=
  const int(5)
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
