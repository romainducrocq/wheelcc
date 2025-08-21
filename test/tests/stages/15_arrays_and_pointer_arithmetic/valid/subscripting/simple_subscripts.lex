-- Lexing ... OK
+
+
@@ Tokens @@
List[541]:
  int
  identifier(integer_types)
  (
  unsigned
  *
  identifier(arr)
  ,
  unsigned
  identifier(expected)
  )
  {
  unsigned
  identifier(val1)
  =
  identifier(arr)
  [
  const int(5)
  ]
  ;
  unsigned
  identifier(val2)
  =
  identifier(arr)
  [
  const unsigned int(5u)
  ]
  ;
  unsigned
  identifier(val3)
  =
  identifier(arr)
  [
  const long(5l)
  ]
  ;
  unsigned
  identifier(val4)
  =
  identifier(arr)
  [
  const unsigned long(5ul)
  ]
  ;
  if
  (
  identifier(val1)
  !=
  identifier(expected)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(val2)
  !=
  identifier(expected)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(val3)
  !=
  identifier(expected)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(val4)
  !=
  identifier(expected)
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
  int
  identifier(reverse_subscript)
  (
  long
  *
  identifier(arr)
  ,
  long
  identifier(expected)
  )
  {
  if
  (
  identifier(arr)
  [
  const int(3)
  ]
  !=
  identifier(expected)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  const int(3)
  [
  identifier(arr)
  ]
  !=
  identifier(expected)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  &
  const int(3)
  [
  identifier(arr)
  ]
  !=
  &
  identifier(arr)
  [
  const int(3)
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
  static
  double
  identifier(static_array)
  [
  const int(3)
  ]
  =
  {
  const double(0.1)
  ,
  const double(0.2)
  ,
  const double(0.3)
  }
  ;
  int
  identifier(subscript_static)
  (
  void
  )
  {
  if
  (
  identifier(static_array)
  [
  const int(0)
  ]
  !=
  const double(0.1)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(static_array)
  [
  const int(1)
  ]
  !=
  const double(0.2)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(static_array)
  [
  const int(2)
  ]
  !=
  const double(0.3)
  )
  {
  return
  const int(10)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(update_element)
  (
  int
  *
  identifier(arr)
  ,
  int
  identifier(expected)
  )
  {
  identifier(arr)
  [
  const int(10)
  ]
  =
  identifier(arr)
  [
  const int(10)
  ]
  *
  const int(2)
  ;
  if
  (
  identifier(arr)
  [
  const int(10)
  ]
  !=
  identifier(expected)
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
  int
  *
  identifier(increment_static_element)
  (
  void
  )
  {
  static
  int
  identifier(arr)
  [
  const int(4)
  ]
  ;
  identifier(arr)
  [
  const int(3)
  ]
  =
  identifier(arr)
  [
  const int(3)
  ]
  +
  const int(1)
  ;
  return
  identifier(arr)
  ;
  }
  int
  identifier(check_increment_static_element)
  (
  void
  )
  {
  int
  *
  identifier(arr1)
  =
  identifier(increment_static_element)
  (
  )
  ;
  if
  (
  identifier(arr1)
  [
  const int(3)
  ]
  !=
  const int(1)
  )
  {
  return
  const int(12)
  ;
  }
  if
  (
  identifier(arr1)
  [
  const int(0)
  ]
  ||
  identifier(arr1)
  [
  const int(1)
  ]
  ||
  identifier(arr1)
  [
  const int(2)
  ]
  )
  {
  return
  const int(13)
  ;
  }
  int
  *
  identifier(arr2)
  =
  identifier(increment_static_element)
  (
  )
  ;
  if
  (
  identifier(arr1)
  !=
  identifier(arr2)
  )
  {
  return
  const int(14)
  ;
  }
  if
  (
  identifier(arr1)
  [
  const int(3)
  ]
  !=
  const int(2)
  )
  {
  return
  const int(15)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  unsigned
  int
  identifier(unsigned_arr)
  [
  const int(6)
  ]
  =
  {
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const unsigned int(7u)
  }
  ;
  int
  identifier(check)
  =
  identifier(integer_types)
  (
  identifier(unsigned_arr)
  ,
  const unsigned int(7u)
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
  long
  int
  identifier(long_arr)
  [
  const int(4)
  ]
  =
  {
  const int(100)
  ,
  const int(102)
  ,
  const int(104)
  ,
  const int(106)
  }
  ;
  identifier(check)
  =
  identifier(reverse_subscript)
  (
  identifier(long_arr)
  ,
  const int(106)
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
  identifier(check)
  =
  identifier(subscript_static)
  (
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
  int
  identifier(int_arr)
  [
  const int(11)
  ]
  =
  {
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(15)
  }
  ;
  identifier(check)
  =
  identifier(update_element)
  (
  identifier(int_arr)
  ,
  const int(30)
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
  identifier(check)
  =
  identifier(check_increment_static_element)
  (
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
  return
  const int(0)
  ;
  }
