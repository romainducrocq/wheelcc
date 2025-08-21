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
  const int(6)
  ]
  =
  {
  -
  const int(10)
  ,
  const int(10)
  ,
  -
  const int(11)
  ,
  const int(11)
  ,
  -
  const int(12)
  ,
  const int(12)
  }
  ;
  if
  (
  (
  identifier(arr)
  [
  const int(0)
  ]
  &
  identifier(arr)
  [
  const int(5)
  ]
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
  (
  identifier(arr)
  [
  const int(1)
  ]
  |
  identifier(arr)
  [
  const int(4)
  ]
  )
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
  (
  identifier(arr)
  [
  const int(2)
  ]
  ^
  identifier(arr)
  [
  const int(3)
  ]
  )
  !=
  -
  const int(2)
  )
  {
  return
  const int(3)
  ;
  }
  identifier(arr)
  [
  const int(0)
  ]
  =
  const int(2041302511)
  ;
  if
  (
  (
  identifier(arr)
  [
  const int(0)
  ]
  >>
  identifier(arr)
  [
  const int(1)
  ]
  )
  !=
  const int(1993459)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  (
  identifier(arr)
  [
  const int(5)
  ]
  <<
  const int(3)
  )
  !=
  const int(96)
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
