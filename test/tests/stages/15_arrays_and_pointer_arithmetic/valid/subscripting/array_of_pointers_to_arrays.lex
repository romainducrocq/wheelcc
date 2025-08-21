-- Lexing ... OK
+
+
@@ Tokens @@
List[222]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(0)
  ;
  int
  identifier(y)
  =
  const int(1)
  ;
  int
  identifier(z)
  =
  const int(2)
  ;
  int
  *
  identifier(arr)
  [
  const int(3)
  ]
  =
  {
  &
  identifier(x)
  ,
  &
  identifier(y)
  ,
  &
  identifier(z)
  }
  ;
  int
  *
  identifier(arr2)
  [
  const int(3)
  ]
  =
  {
  &
  identifier(z)
  ,
  &
  identifier(y)
  ,
  &
  identifier(x)
  }
  ;
  int
  *
  (
  *
  identifier(array_of_pointers)
  [
  const int(3)
  ]
  )
  [
  const int(3)
  ]
  =
  {
  &
  identifier(arr)
  ,
  &
  identifier(arr2)
  ,
  &
  identifier(arr)
  }
  ;
  if
  (
  identifier(array_of_pointers)
  [
  const int(0)
  ]
  !=
  (
  int
  *
  (
  *
  )
  [
  const int(3)
  ]
  )
  identifier(arr)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(array_of_pointers)
  [
  const int(1)
  ]
  !=
  (
  int
  *
  (
  *
  )
  [
  const int(3)
  ]
  )
  identifier(arr2)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(array_of_pointers)
  [
  const int(2)
  ]
  !=
  (
  int
  *
  (
  *
  )
  [
  const int(3)
  ]
  )
  identifier(arr)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(array_of_pointers)
  [
  const int(1)
  ]
  [
  const int(0)
  ]
  [
  const int(0)
  ]
  !=
  &
  identifier(z)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(array_of_pointers)
  [
  const int(1)
  ]
  [
  const int(0)
  ]
  [
  const int(1)
  ]
  !=
  &
  identifier(y)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(array_of_pointers)
  [
  const int(2)
  ]
  [
  const int(0)
  ]
  [
  const int(2)
  ]
  [
  const int(0)
  ]
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
