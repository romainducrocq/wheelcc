-- Lexing ... OK
+
+
@@ Tokens @@
List[583]:
  void
  *
  identifier(malloc)
  (
  unsigned
  long
  identifier(size)
  )
  ;
  void
  identifier(free)
  (
  void
  *
  identifier(ptr)
  )
  ;
  int
  identifier(memcmp)
  (
  void
  *
  identifier(s1)
  ,
  void
  *
  identifier(s2)
  ,
  unsigned
  long
  identifier(n)
  )
  ;
  void
  *
  identifier(return_ptr)
  (
  char
  *
  identifier(i)
  )
  {
  return
  identifier(i)
  +
  const int(3)
  ;
  }
  int
  identifier(check_char_ptr_argument)
  (
  char
  *
  identifier(pointer)
  ,
  char
  identifier(expected_val)
  )
  {
  return
  *
  identifier(pointer)
  ==
  identifier(expected_val)
  ;
  }
  int
  *
  identifier(return_void_ptr_as_int_ptr)
  (
  void
  *
  identifier(pointer)
  )
  {
  return
  identifier(pointer)
  ;
  }
  double
  *
  identifier(get_dbl_array)
  (
  unsigned
  long
  identifier(n)
  )
  {
  return
  (
  double
  *
  )
  identifier(malloc)
  (
  identifier(n)
  *
  sizeof
  (
  double
  )
  )
  ;
  }
  void
  identifier(set_doubles)
  (
  double
  *
  identifier(array)
  ,
  unsigned
  long
  identifier(n)
  ,
  double
  identifier(d)
  )
  {
  for
  (
  unsigned
  long
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  identifier(n)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  identifier(array)
  [
  identifier(i)
  ]
  =
  identifier(d)
  ;
  }
  return
  ;
  }
  void
  *
  identifier(return_dbl_ptr_as_void_ptr)
  (
  double
  *
  identifier(ptr)
  )
  {
  return
  identifier(ptr)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  void
  *
  identifier(four_bytes)
  =
  identifier(malloc)
  (
  const int(4)
  )
  ;
  int
  *
  identifier(int_ptr)
  =
  identifier(four_bytes)
  ;
  *
  identifier(int_ptr)
  =
  -
  const int(1)
  ;
  if
  (
  !
  identifier(check_char_ptr_argument)
  (
  identifier(four_bytes)
  ,
  -
  const int(1)
  )
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(return_void_ptr_as_int_ptr)
  (
  identifier(four_bytes)
  )
  !=
  identifier(int_ptr)
  )
  {
  return
  const int(2)
  ;
  }
  double
  *
  identifier(dbl_ptr)
  =
  identifier(four_bytes)
  ;
  int
  (
  *
  identifier(complicated_ptr)
  )
  [
  const int(3)
  ]
  [
  const int(2)
  ]
  [
  const int(5)
  ]
  =
  identifier(four_bytes)
  ;
  long
  *
  identifier(long_ptr)
  =
  identifier(four_bytes)
  ;
  if
  (
  identifier(dbl_ptr)
  !=
  identifier(four_bytes)
  ||
  identifier(complicated_ptr)
  !=
  identifier(four_bytes)
  ||
  identifier(long_ptr)
  !=
  identifier(four_bytes)
  )
  {
  return
  const int(3)
  ;
  }
  identifier(free)
  (
  identifier(four_bytes)
  )
  ;
  double
  *
  identifier(dbl_array)
  =
  identifier(get_dbl_array)
  (
  const int(5)
  )
  ;
  void
  *
  identifier(void_array)
  =
  identifier(dbl_array)
  ;
  identifier(set_doubles)
  (
  identifier(void_array)
  ,
  const int(5)
  ,
  const double(4.0)
  )
  ;
  if
  (
  identifier(dbl_array)
  [
  const int(3)
  ]
  !=
  const double(4.0)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(return_dbl_ptr_as_void_ptr)
  (
  identifier(dbl_array)
  )
  !=
  identifier(void_array)
  )
  {
  return
  const int(5)
  ;
  }
  void
  *
  identifier(some_other_ptr)
  =
  const int(0)
  ;
  identifier(some_other_ptr)
  =
  identifier(dbl_array)
  ;
  if
  (
  identifier(some_other_ptr)
  !=
  identifier(void_array)
  )
  {
  return
  const int(6)
  ;
  }
  identifier(some_other_ptr)
  =
  &
  identifier(some_other_ptr)
  ;
  if
  (
  identifier(some_other_ptr)
  ==
  identifier(void_array)
  )
  {
  return
  const int(7)
  ;
  }
  identifier(complicated_ptr)
  =
  const int(0)
  ;
  identifier(some_other_ptr)
  =
  identifier(complicated_ptr)
  ;
  if
  (
  identifier(some_other_ptr)
  )
  {
  return
  const int(8)
  ;
  }
  identifier(free)
  (
  identifier(dbl_array)
  )
  ;
  long
  *
  identifier(long_ptr_array)
  [
  const int(3)
  ]
  =
  {
  identifier(malloc)
  (
  sizeof
  (
  long
  )
  )
  ,
  identifier(malloc)
  (
  sizeof
  (
  long
  )
  )
  ,
  identifier(malloc)
  (
  sizeof
  (
  long
  )
  )
  }
  ;
  *
  identifier(long_ptr_array)
  [
  const int(0)
  ]
  =
  const long(100l)
  ;
  *
  identifier(long_ptr_array)
  [
  const int(1)
  ]
  =
  const long(200l)
  ;
  *
  identifier(long_ptr_array)
  [
  const int(2)
  ]
  =
  const long(300l)
  ;
  long
  identifier(sum)
  =
  (
  *
  identifier(long_ptr_array)
  [
  const int(0)
  ]
  +
  *
  identifier(long_ptr_array)
  [
  const int(1)
  ]
  +
  *
  identifier(long_ptr_array)
  [
  const int(2)
  ]
  )
  ;
  if
  (
  identifier(sum)
  !=
  const long(600l)
  )
  {
  return
  const int(9)
  ;
  }
  identifier(free)
  (
  identifier(long_ptr_array)
  [
  const int(0)
  ]
  )
  ;
  identifier(free)
  (
  identifier(long_ptr_array)
  [
  const int(1)
  ]
  )
  ;
  identifier(free)
  (
  identifier(long_ptr_array)
  [
  const int(2)
  ]
  )
  ;
  long
  identifier(arr1)
  [
  const int(3)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  }
  ;
  long
  identifier(arr2)
  [
  const int(3)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  }
  ;
  long
  identifier(arr3)
  [
  const int(3)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(4)
  }
  ;
  if
  (
  identifier(memcmp)
  (
  identifier(arr1)
  ,
  identifier(arr2)
  ,
  sizeof
  identifier(arr1)
  )
  !=
  const int(0)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  identifier(memcmp)
  (
  identifier(arr1)
  ,
  identifier(arr3)
  ,
  sizeof
  identifier(arr2)
  )
  !=
  -
  const int(1)
  )
  {
  return
  const int(11)
  ;
  }
  return
  const int(0)
  ;
  }
