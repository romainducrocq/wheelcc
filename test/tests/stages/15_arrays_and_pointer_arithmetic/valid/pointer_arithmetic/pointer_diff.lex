-- Lexing ... OK
+
+
@@ Tokens @@
List[256]:
  int
  identifier(get_ptr_diff)
  (
  int
  *
  identifier(ptr1)
  ,
  int
  *
  identifier(ptr2)
  )
  {
  return
  (
  identifier(ptr2)
  -
  identifier(ptr1)
  )
  ;
  }
  int
  identifier(get_long_ptr_diff)
  (
  long
  *
  identifier(ptr1)
  ,
  long
  *
  identifier(ptr2)
  )
  {
  return
  (
  identifier(ptr2)
  -
  identifier(ptr1)
  )
  ;
  }
  int
  identifier(get_multidim_ptr_diff)
  (
  double
  (
  *
  identifier(ptr1)
  )
  [
  const int(3)
  ]
  [
  const int(5)
  ]
  ,
  double
  (
  *
  identifier(ptr2)
  )
  [
  const int(3)
  ]
  [
  const int(5)
  ]
  )
  {
  return
  (
  identifier(ptr2)
  -
  identifier(ptr1)
  )
  ;
  }
  int
  identifier(get_multidim_ptr_diff_2)
  (
  double
  (
  *
  identifier(ptr1)
  )
  [
  const int(5)
  ]
  ,
  double
  (
  *
  identifier(ptr2)
  )
  [
  const int(5)
  ]
  )
  {
  return
  (
  identifier(ptr2)
  -
  identifier(ptr1)
  )
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(arr)
  [
  const int(5)
  ]
  =
  {
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
  int
  *
  identifier(end_of_array)
  =
  identifier(arr)
  +
  const int(5)
  ;
  if
  (
  identifier(get_ptr_diff)
  (
  identifier(arr)
  ,
  identifier(end_of_array)
  )
  !=
  const int(5)
  )
  {
  return
  const int(1)
  ;
  }
  long
  identifier(long_arr)
  [
  const int(8)
  ]
  ;
  if
  (
  identifier(get_long_ptr_diff)
  (
  identifier(long_arr)
  +
  const int(3)
  ,
  identifier(long_arr)
  )
  !=
  -
  const int(3)
  )
  {
  return
  const int(2)
  ;
  }
  static
  double
  identifier(multidim)
  [
  const int(6)
  ]
  [
  const int(7)
  ]
  [
  const int(3)
  ]
  [
  const int(5)
  ]
  ;
  if
  (
  identifier(get_multidim_ptr_diff)
  (
  identifier(multidim)
  [
  const int(2)
  ]
  +
  const int(1)
  ,
  identifier(multidim)
  [
  const int(2)
  ]
  +
  const int(4)
  )
  !=
  const int(3)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(get_multidim_ptr_diff_2)
  (
  identifier(multidim)
  [
  const int(2)
  ]
  [
  const int(2)
  ]
  +
  const int(2)
  ,
  identifier(multidim)
  [
  const int(2)
  ]
  [
  const int(2)
  ]
  )
  !=
  -
  const int(2)
  )
  {
  return
  const int(4)
  ;
  }
  return
  const int(0)
  ;
  }
