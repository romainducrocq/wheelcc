-- Lexing ... OK
+
+
@@ Tokens @@
List[298]:
  int
  identifier(main)
  (
  void
  )
  {
  long
  identifier(arr)
  [
  const int(2)
  ]
  [
  const int(3)
  ]
  [
  const int(4)
  ]
  =
  {
  {
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  ,
  const int(4)
  }
  ,
  {
  const int(5)
  ,
  const int(6)
  ,
  const int(7)
  ,
  const int(8)
  }
  ,
  {
  const int(9)
  ,
  const int(10)
  ,
  const int(11)
  ,
  const int(12)
  }
  }
  ,
  {
  {
  const int(13)
  ,
  const int(14)
  ,
  const int(15)
  ,
  const int(16)
  }
  ,
  {
  const int(17)
  ,
  const int(18)
  ,
  const int(19)
  ,
  const int(20)
  }
  ,
  {
  const int(21)
  ,
  const int(22)
  ,
  const int(23)
  ,
  const int(24)
  }
  }
  }
  ;
  long
  (
  *
  identifier(outer_ptr)
  )
  [
  const int(3)
  ]
  [
  const int(4)
  ]
  =
  identifier(arr)
  +
  const int(1)
  ;
  if
  (
  identifier(outer_ptr)
  --
  !=
  &
  identifier(arr)
  [
  const int(1)
  ]
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(outer_ptr)
  [
  const int(0)
  ]
  [
  const int(1)
  ]
  !=
  identifier(arr)
  [
  const int(0)
  ]
  [
  const int(1)
  ]
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  (
  ++
  identifier(outer_ptr)
  )
  [
  const int(0)
  ]
  [
  const int(2)
  ]
  [
  const int(3)
  ]
  !=
  const int(24)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(outer_ptr)
  [
  const int(0)
  ]
  [
  const int(2)
  ]
  [
  const int(3)
  ]
  !=
  const int(24)
  )
  {
  return
  const int(4)
  ;
  }
  long
  (
  *
  identifier(inner_ptr)
  )
  [
  const int(4)
  ]
  =
  identifier(arr)
  [
  const int(0)
  ]
  +
  const int(1)
  ;
  if
  (
  identifier(inner_ptr)
  ++
  [
  const int(0)
  ]
  [
  const int(2)
  ]
  !=
  const int(7)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(inner_ptr)
  [
  const int(0)
  ]
  [
  const int(2)
  ]
  !=
  const int(11)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  (
  --
  identifier(inner_ptr)
  )
  [
  const int(0)
  ]
  [
  const int(1)
  ]
  !=
  const int(6)
  )
  {
  return
  const int(7)
  ;
  }
  long
  *
  identifier(scalar_ptr)
  =
  identifier(arr)
  [
  const int(1)
  ]
  [
  const int(2)
  ]
  ;
  if
  (
  identifier(scalar_ptr)
  --
  [
  const int(2)
  ]
  !=
  const int(23)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(scalar_ptr)
  [
  const int(2)
  ]
  !=
  const int(22)
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
