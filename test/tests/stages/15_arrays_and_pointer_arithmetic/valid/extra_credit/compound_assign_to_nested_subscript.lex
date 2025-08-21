-- Lexing ... OK
+
+
@@ Tokens @@
List[287]:
  long
  identifier(long_nested_arr)
  [
  const int(2)
  ]
  [
  const int(3)
  ]
  =
  {
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  }
  ,
  {
  const int(4)
  ,
  const int(5)
  ,
  const int(6)
  }
  }
  ;
  double
  identifier(dbl_nested_arr)
  [
  const int(3)
  ]
  [
  const int(2)
  ]
  =
  {
  {
  const double(100.0)
  ,
  const double(101.0)
  }
  ,
  {
  const double(102.0)
  ,
  const double(103.0)
  }
  ,
  {
  const double(104.0)
  ,
  const double(105.0)
  }
  }
  ;
  unsigned
  identifier(unsigned_index)
  =
  const int(10)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  (
  identifier(long_nested_arr)
  [
  const int(1)
  ]
  [
  identifier(unsigned_index)
  -
  const int(8)
  ]
  *=
  -
  const int(1)
  )
  !=
  -
  const int(6)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(long_nested_arr)
  [
  const int(1)
  ]
  [
  const int(2)
  ]
  !=
  -
  const int(6)
  )
  {
  return
  const int(2)
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
  const int(2)
  ;
  identifier(i)
  +=
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
  const int(3)
  ;
  identifier(j)
  +=
  const int(1)
  )
  {
  if
  (
  identifier(i)
  ==
  const int(1)
  &&
  identifier(j)
  ==
  const int(2)
  )
  {
  break
  ;
  }
  long
  identifier(expected)
  =
  identifier(i)
  *
  const int(3)
  +
  identifier(j)
  +
  const int(1)
  ;
  if
  (
  identifier(long_nested_arr)
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
  const int(3)
  ;
  }
  }
  }
  if
  (
  (
  identifier(dbl_nested_arr)
  [
  const int(1)
  ]
  [
  const int(1)
  ]
  +=
  const double(100.0)
  )
  !=
  const double(203.0)
  )
  {
  return
  const int(4)
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
  +=
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
  +=
  const int(1)
  )
  {
  if
  (
  identifier(i)
  ==
  const int(1)
  &&
  identifier(j)
  ==
  const int(1)
  )
  {
  continue
  ;
  }
  int
  identifier(expected)
  =
  const int(100)
  +
  identifier(i)
  *
  const int(2)
  +
  identifier(j)
  ;
  if
  (
  identifier(dbl_nested_arr)
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
  const int(5)
  ;
  }
  }
  }
  return
  const int(0)
  ;
  }
