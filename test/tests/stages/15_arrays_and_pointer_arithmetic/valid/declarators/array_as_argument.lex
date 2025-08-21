-- Lexing ... OK
+
+
@@ Tokens @@
List[342]:
  int
  identifier(array_param)
  (
  int
  identifier(a)
  [
  const int(5)
  ]
  )
  {
  identifier(a)
  [
  const int(4)
  ]
  =
  const int(0)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(nested_array_param)
  (
  int
  identifier(a)
  [
  const int(2)
  ]
  [
  const int(3)
  ]
  )
  {
  identifier(a)
  [
  const int(1)
  ]
  [
  const int(1)
  ]
  =
  const int(1)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(array_param)
  (
  int
  identifier(a)
  [
  const int(2)
  ]
  )
  ;
  int
  identifier(nested_array_param)
  (
  int
  (
  *
  identifier(a)
  )
  [
  const int(3)
  ]
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(array_param)
  (
  int
  identifier(a)
  [
  const int(6)
  ]
  )
  ;
  int
  identifier(nested_array_param)
  (
  int
  identifier(a)
  [
  const int(5)
  ]
  [
  const int(3)
  ]
  )
  ;
  int
  identifier(arr)
  [
  const int(8)
  ]
  =
  {
  const int(8)
  ,
  const int(7)
  ,
  const int(6)
  ,
  const int(5)
  ,
  const int(4)
  ,
  const int(3)
  ,
  const int(2)
  ,
  const int(1)
  }
  ;
  identifier(array_param)
  (
  identifier(arr)
  )
  ;
  if
  (
  identifier(arr)
  [
  const int(4)
  ]
  )
  {
  return
  const int(1)
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
  const int(8)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  if
  (
  identifier(i)
  !=
  const int(4)
  &&
  identifier(arr)
  [
  identifier(i)
  ]
  !=
  const int(8)
  -
  identifier(i)
  )
  return
  const int(2)
  ;
  }
  int
  identifier(nested_arr)
  [
  const int(4)
  ]
  [
  const int(3)
  ]
  =
  {
  {
  -
  const int(1)
  ,
  -
  const int(1)
  ,
  -
  const int(1)
  }
  ,
  {
  -
  const int(2)
  ,
  -
  const int(2)
  ,
  -
  const int(2)
  }
  ,
  {
  -
  const int(3)
  ,
  -
  const int(3)
  ,
  -
  const int(3)
  }
  ,
  {
  -
  const int(4)
  ,
  -
  const int(4)
  ,
  -
  const int(4)
  }
  }
  ;
  identifier(nested_array_param)
  (
  identifier(nested_arr)
  )
  ;
  if
  (
  identifier(nested_arr)
  [
  const int(1)
  ]
  [
  const int(1)
  ]
  !=
  const int(1)
  )
  {
  return
  const int(3)
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
  =
  identifier(i)
  +
  const int(1)
  )
  {
  int
  identifier(expected)
  =
  -
  const int(1)
  -
  identifier(i)
  ;
  for
  (
  int
  identifier(j)
  =
  const int(0)
  ;
  identifier(j)
  <
  const int(3)
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
  (
  identifier(i)
  !=
  const int(1)
  ||
  identifier(j)
  !=
  const int(1)
  )
  &&
  (
  identifier(nested_arr)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  !=
  identifier(expected)
  )
  )
  {
  return
  const int(4)
  ;
  }
  }
  }
  return
  const int(0)
  ;
  }
  int
  identifier(array_param)
  (
  int
  *
  identifier(a)
  )
  ;
