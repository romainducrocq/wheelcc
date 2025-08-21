-- Lexing ... OK
+
+
@@ Tokens @@
List[153]:
  int
  identifier(set_nth_element)
  (
  double
  *
  identifier(arr)
  ,
  int
  identifier(idx)
  )
  {
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(5)
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
  identifier(arr)
  [
  identifier(i)
  ]
  )
  {
  return
  const int(1)
  ;
  }
  }
  identifier(arr)
  [
  identifier(idx)
  ]
  =
  const int(8)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(set_nested_element)
  (
  int
  (
  *
  identifier(arr)
  )
  [
  const int(2)
  ]
  ,
  int
  identifier(i)
  ,
  int
  identifier(j)
  )
  {
  for
  (
  int
  identifier(x)
  =
  const int(0)
  ;
  identifier(x)
  <
  const int(3)
  ;
  identifier(x)
  =
  identifier(x)
  +
  const int(1)
  )
  {
  for
  (
  int
  identifier(y)
  =
  const int(0)
  ;
  identifier(y)
  <
  const int(2)
  ;
  identifier(y)
  =
  identifier(y)
  +
  const int(1)
  )
  {
  int
  identifier(expected)
  =
  -
  const int(10)
  +
  const int(2)
  *
  identifier(x)
  +
  identifier(y)
  ;
  if
  (
  identifier(arr)
  [
  identifier(x)
  ]
  [
  identifier(y)
  ]
  !=
  identifier(expected)
  )
  {
  return
  const int(4)
  ;
  }
  }
  }
  identifier(arr)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  =
  const int(10)
  ;
  return
  const int(0)
  ;
  }
