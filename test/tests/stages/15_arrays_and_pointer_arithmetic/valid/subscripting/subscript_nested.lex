-- Lexing ... OK
+
+
@@ Tokens @@
List[513]:
  int
  identifier(read_nested)
  (
  int
  identifier(nested_arr)
  [
  const int(2)
  ]
  [
  const int(3)
  ]
  ,
  int
  identifier(i)
  ,
  int
  identifier(j)
  ,
  int
  identifier(expected)
  )
  {
  return
  (
  identifier(nested_arr)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  ==
  identifier(expected)
  )
  ;
  }
  int
  identifier(write_nested)
  (
  int
  identifier(nested_arr)
  [
  const int(2)
  ]
  [
  const int(3)
  ]
  ,
  int
  identifier(i)
  ,
  int
  identifier(j)
  ,
  int
  identifier(new_val)
  )
  {
  identifier(nested_arr)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  =
  identifier(new_val)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(read_nested_negated)
  (
  int
  (
  *
  identifier(nested_arr)
  )
  [
  const int(3)
  ]
  ,
  int
  identifier(i)
  ,
  int
  identifier(j)
  ,
  int
  identifier(expected)
  )
  {
  return
  (
  identifier(nested_arr)
  [
  -
  identifier(i)
  ]
  [
  identifier(j)
  ]
  ==
  identifier(expected)
  )
  ;
  }
  int
  identifier(get_nested_addr)
  (
  int
  identifier(nested_arr)
  [
  const int(2)
  ]
  [
  const int(3)
  ]
  ,
  int
  identifier(i)
  ,
  int
  identifier(j)
  ,
  int
  *
  identifier(expected)
  )
  {
  return
  &
  identifier(nested_arr)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  ==
  identifier(expected)
  ;
  }
  static
  int
  identifier(nested_arr)
  [
  const int(4)
  ]
  [
  const int(3)
  ]
  [
  const int(5)
  ]
  =
  {
  {
  {
  const int(1)
  ,
  const int(2)
  }
  ,
  {
  const int(3)
  }
  }
  ,
  {
  {
  const int(4)
  }
  ,
  {
  const int(5)
  }
  }
  }
  ;
  int
  identifier(read_static_nested)
  (
  int
  identifier(i)
  ,
  int
  identifier(j)
  ,
  int
  identifier(k)
  ,
  int
  identifier(expected)
  )
  {
  return
  identifier(nested_arr)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  [
  identifier(k)
  ]
  ==
  identifier(expected)
  ;
  }
  int
  (
  *
  identifier(get_array)
  (
  void
  )
  )
  [
  const int(3)
  ]
  [
  const int(5)
  ]
  {
  return
  identifier(nested_arr)
  ;
  }
  int
  identifier(write_nested_complex)
  (
  int
  identifier(i)
  ,
  int
  identifier(j)
  ,
  int
  identifier(k)
  ,
  int
  identifier(val)
  )
  {
  identifier(get_array)
  (
  )
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  [
  identifier(k)
  ]
  =
  identifier(val)
  ;
  return
  const int(0)
  ;
  }
  int
  *
  identifier(get_subarray)
  (
  int
  identifier(nested)
  [
  const int(2)
  ]
  [
  const int(3)
  ]
  ,
  int
  identifier(i)
  )
  {
  return
  identifier(nested)
  [
  identifier(i)
  ]
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(nested_arr)
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
  if
  (
  !
  identifier(read_nested)
  (
  identifier(nested_arr)
  ,
  const int(1)
  ,
  const int(2)
  ,
  const int(6)
  )
  )
  {
  return
  const int(1)
  ;
  }
  identifier(write_nested)
  (
  identifier(nested_arr)
  ,
  const int(1)
  ,
  const int(2)
  ,
  -
  const int(1)
  )
  ;
  if
  (
  identifier(nested_arr)
  [
  const int(1)
  ]
  [
  const int(2)
  ]
  !=
  -
  const int(1)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  !
  identifier(read_nested_negated)
  (
  identifier(nested_arr)
  +
  const int(2)
  ,
  const int(2)
  ,
  const int(0)
  ,
  const int(1)
  )
  )
  {
  return
  const int(3)
  ;
  }
  int
  *
  identifier(ptr)
  =
  (
  identifier(nested_arr)
  [
  const int(0)
  ]
  )
  +
  const int(1)
  ;
  if
  (
  !
  identifier(get_nested_addr)
  (
  identifier(nested_arr)
  ,
  const int(0)
  ,
  const int(1)
  ,
  identifier(ptr)
  )
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  !
  identifier(read_static_nested)
  (
  const int(1)
  ,
  const int(1)
  ,
  const int(0)
  ,
  const int(5)
  )
  )
  {
  return
  const int(5)
  ;
  }
  identifier(write_nested_complex)
  (
  const int(0)
  ,
  const int(2)
  ,
  const int(3)
  ,
  const int(111)
  )
  ;
  if
  (
  identifier(get_array)
  (
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
  const int(111)
  )
  {
  return
  const int(6)
  ;
  }
  int
  *
  identifier(row_1)
  =
  identifier(get_subarray)
  (
  identifier(nested_arr)
  ,
  const int(1)
  )
  ;
  if
  (
  identifier(row_1)
  +
  const int(1)
  !=
  &
  identifier(nested_arr)
  [
  const int(1)
  ]
  [
  const int(1)
  ]
  )
  {
  return
  const int(7)
  ;
  }
  return
  const int(0)
  ;
  }
