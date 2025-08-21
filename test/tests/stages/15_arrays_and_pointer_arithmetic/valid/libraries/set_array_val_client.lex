-- Lexing ... OK
+
+
@@ Tokens @@
List[276]:
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
  ;
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
  ;
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(arr)
  [
  const int(5)
  ]
  =
  {
  const double(0.0)
  ,
  const double(0.0)
  ,
  const double(0.0)
  ,
  const double(0.0)
  ,
  const double(0.0)
  }
  ;
  int
  identifier(check)
  =
  identifier(set_nth_element)
  (
  identifier(arr)
  ,
  const int(4)
  )
  ;
  if
  (
  identifier(check)
  )
  {
  return
  identifier(check)
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
  if
  (
  identifier(arr)
  [
  identifier(i)
  ]
  !=
  const int(0)
  )
  {
  return
  const int(2)
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
  const int(8)
  )
  return
  const int(3)
  ;
  int
  identifier(nested_arr)
  [
  const int(3)
  ]
  [
  const int(2)
  ]
  =
  {
  {
  -
  const int(10)
  ,
  -
  const int(9)
  }
  ,
  {
  -
  const int(8)
  ,
  -
  const int(7)
  }
  ,
  {
  -
  const int(6)
  ,
  -
  const int(5)
  }
  }
  ;
  identifier(check)
  =
  identifier(set_nested_element)
  (
  identifier(nested_arr)
  ,
  const int(2)
  ,
  const int(1)
  )
  ;
  if
  (
  identifier(check)
  )
  {
  return
  identifier(check)
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
  const int(2)
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
  identifier(i)
  ==
  const int(2)
  &&
  identifier(j)
  ==
  const int(1)
  )
  {
  if
  (
  identifier(nested_arr)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  !=
  const int(10)
  )
  {
  return
  const int(5)
  ;
  }
  }
  else
  {
  int
  identifier(expected)
  =
  -
  const int(10)
  +
  const int(2)
  *
  identifier(i)
  +
  identifier(j)
  ;
  if
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
  {
  return
  const int(6)
  ;
  }
  }
  }
  }
  return
  const int(0)
  ;
  }
