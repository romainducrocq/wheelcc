-- Lexing ... OK
+
+
@@ Tokens @@
List[198]:
  static
  long
  identifier(nested_arr)
  [
  const int(3)
  ]
  [
  const int(4)
  ]
  [
  const int(5)
  ]
  =
  {
  {
  {
  const int(10)
  ,
  const int(9)
  ,
  const int(8)
  }
  ,
  {
  const int(1)
  ,
  const int(2)
  }
  }
  ,
  {
  {
  const int(100)
  ,
  const int(99)
  ,
  const int(98)
  }
  }
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  long
  (
  *
  identifier(outer_ptr)
  )
  [
  const int(4)
  ]
  [
  const int(5)
  ]
  =
  identifier(nested_arr)
  ;
  identifier(outer_ptr)
  +=
  const int(1)
  ;
  if
  (
  identifier(outer_ptr)
  !=
  identifier(nested_arr)
  +
  const int(1)
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
  const int(0)
  ]
  [
  const int(0)
  ]
  !=
  const int(100)
  )
  {
  return
  const int(2)
  ;
  }
  long
  (
  *
  identifier(inner_ptr)
  )
  [
  const int(5)
  ]
  =
  identifier(nested_arr)
  [
  const int(0)
  ]
  +
  const int(4)
  ;
  identifier(inner_ptr)
  -=
  const int(3)
  ;
  if
  (
  identifier(inner_ptr)
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
  const int(3)
  ;
  }
  unsigned
  long
  identifier(idx)
  =
  identifier(nested_arr)
  [
  const int(0)
  ]
  [
  const int(0)
  ]
  [
  const int(0)
  ]
  -
  const int(9)
  ;
  if
  (
  (
  identifier(inner_ptr)
  +=
  identifier(idx)
  )
  !=
  &
  identifier(nested_arr)
  [
  const int(0)
  ]
  [
  const int(2)
  ]
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  (
  identifier(inner_ptr)
  [
  -
  const int(2)
  ]
  [
  const int(1)
  ]
  !=
  const int(9)
  )
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
