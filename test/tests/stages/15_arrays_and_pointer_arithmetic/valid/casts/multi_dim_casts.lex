-- Lexing ... OK
+
+
@@ Tokens @@
List[175]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(multi_dim)
  [
  const int(2)
  ]
  [
  const int(3)
  ]
  =
  {
  {
  const int(0)
  ,
  const int(1)
  ,
  const int(2)
  }
  ,
  {
  const int(3)
  ,
  const int(4)
  ,
  const int(5)
  }
  }
  ;
  int
  (
  *
  identifier(array_pointer)
  )
  [
  const int(2)
  ]
  [
  const int(3)
  ]
  =
  &
  identifier(multi_dim)
  ;
  int
  (
  *
  identifier(row_pointer)
  )
  [
  const int(3)
  ]
  =
  (
  int
  (
  *
  )
  [
  const int(3)
  ]
  )
  identifier(array_pointer)
  ;
  if
  (
  identifier(row_pointer)
  !=
  identifier(multi_dim)
  )
  {
  return
  const int(1)
  ;
  }
  identifier(row_pointer)
  =
  identifier(row_pointer)
  +
  const int(1)
  ;
  if
  (
  identifier(row_pointer)
  [
  const int(0)
  ]
  [
  const int(1)
  ]
  !=
  const int(4)
  )
  {
  return
  const int(2)
  ;
  }
  int
  *
  identifier(elem_ptr)
  =
  (
  int
  *
  )
  identifier(row_pointer)
  ;
  if
  (
  *
  identifier(elem_ptr)
  !=
  const int(3)
  )
  {
  return
  const int(3)
  ;
  }
  identifier(elem_ptr)
  =
  identifier(elem_ptr)
  +
  const int(2)
  ;
  if
  (
  *
  identifier(elem_ptr)
  !=
  const int(5)
  )
  {
  return
  const int(4)
  ;
  }
  identifier(row_pointer)
  =
  identifier(row_pointer)
  -
  const int(1)
  ;
  if
  (
  (
  int
  (
  *
  )
  [
  const int(2)
  ]
  [
  const int(3)
  ]
  )
  identifier(row_pointer)
  !=
  identifier(array_pointer)
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
