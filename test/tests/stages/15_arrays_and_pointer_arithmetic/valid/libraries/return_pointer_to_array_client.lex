-- Lexing ... OK
+
+
@@ Tokens @@
List[186]:
  long
  (
  *
  identifier(return_row)
  (
  long
  (
  *
  identifier(arr)
  )
  [
  const int(3)
  ]
  [
  const int(4)
  ]
  ,
  int
  identifier(idx)
  )
  )
  [
  const int(4)
  ]
  ;
  int
  identifier(main)
  (
  void
  )
  {
  long
  identifier(nested_array)
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
  const int(0)
  }
  }
  ,
  {
  {
  -
  const int(12)
  ,
  -
  const int(13)
  ,
  -
  const int(14)
  ,
  -
  const int(15)
  }
  ,
  {
  -
  const int(16)
  }
  }
  }
  ;
  long
  (
  *
  identifier(row_pointer)
  )
  [
  const int(4)
  ]
  =
  identifier(return_row)
  (
  identifier(nested_array)
  ,
  const int(1)
  )
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(3)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  for
  (
  int
  identifier(j)
  =
  const int(0)
  ;
  identifier(j)
  <
  const int(4)
  ;
  identifier(j)
  =
  identifier(j)
  +
  const int(1)
  )
  {
  if
  (
  identifier(row_pointer)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  !=
  identifier(nested_array)
  [
  const int(1)
  ]
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  )
  {
  return
  const int(1)
  ;
  }
  }
  }
  identifier(row_pointer)
  [
  const int(2)
  ]
  [
  const int(1)
  ]
  =
  const int(100)
  ;
  if
  (
  identifier(nested_array)
  [
  const int(1)
  ]
  [
  const int(2)
  ]
  [
  const int(1)
  ]
  !=
  const int(100)
  )
  {
  return
  const int(2)
  ;
  }
  return
  const int(0)
  ;
  }
