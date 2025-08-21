-- Lexing ... OK
+
+
@@ Tokens @@
List[1007]:
  int
  identifier(test_add_constant_to_pointer)
  (
  void
  )
  {
  long
  identifier(long_arr)
  [
  const int(12)
  ]
  =
  {
  const int(0)
  ,
  const int(0)
  ,
  const int(3)
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
  const int(13)
  }
  ;
  long
  *
  identifier(ptr)
  =
  identifier(long_arr)
  +
  const int(10)
  ;
  return
  *
  identifier(ptr)
  ==
  const int(13)
  ;
  }
  int
  identifier(test_add_negative_index)
  (
  void
  )
  {
  unsigned
  identifier(unsigned_arr)
  [
  const int(12)
  ]
  =
  {
  const int(0)
  ,
  const int(0)
  ,
  const int(2)
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
  const int(42)
  }
  ;
  unsigned
  *
  identifier(end_ptr)
  =
  identifier(unsigned_arr)
  +
  const int(12)
  ;
  unsigned
  *
  identifier(ptr)
  =
  identifier(end_ptr)
  +
  -
  const int(10)
  ;
  return
  *
  identifier(ptr)
  ==
  const int(2)
  ;
  }
  int
  identifier(test_add_pointer_to_int)
  (
  void
  )
  {
  int
  identifier(int_arr)
  [
  const int(5)
  ]
  =
  {
  const int(0)
  ,
  const int(98)
  ,
  const int(99)
  }
  ;
  int
  *
  identifier(ptr1)
  =
  identifier(int_arr)
  +
  const int(2)
  ;
  int
  *
  identifier(ptr2)
  =
  const int(2)
  +
  identifier(int_arr)
  ;
  return
  (
  identifier(ptr1)
  ==
  identifier(ptr2)
  &&
  *
  identifier(ptr2)
  ==
  const int(99)
  )
  ;
  }
  int
  identifier(test_add_different_index_types)
  (
  void
  )
  {
  double
  identifier(double_arr)
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
  const double(6.0)
  }
  ;
  double
  *
  identifier(ptr1)
  =
  identifier(double_arr)
  +
  const int(5)
  ;
  double
  *
  identifier(ptr2)
  =
  identifier(double_arr)
  +
  const long(5l)
  ;
  double
  *
  identifier(ptr3)
  =
  identifier(double_arr)
  +
  const unsigned int(5u)
  ;
  double
  *
  identifier(ptr4)
  =
  identifier(double_arr)
  +
  const unsigned long(5ul)
  ;
  return
  (
  identifier(ptr1)
  ==
  identifier(ptr2)
  &&
  identifier(ptr1)
  ==
  identifier(ptr3)
  &&
  identifier(ptr1)
  ==
  identifier(ptr4)
  &&
  *
  identifier(ptr4)
  ==
  const double(6.0)
  )
  ;
  }
  int
  identifier(test_add_complex_expressions)
  (
  void
  )
  {
  static
  int
  identifier(flag)
  ;
  int
  identifier(i)
  =
  -
  const int(2)
  ;
  int
  *
  identifier(small_int_ptr)
  =
  &
  identifier(i)
  ;
  extern
  int
  identifier(return_one)
  (
  void
  )
  ;
  extern
  int
  *
  identifier(get_elem1_ptr)
  (
  int
  *
  identifier(arr)
  )
  ;
  extern
  int
  *
  identifier(get_elem2_ptr)
  (
  int
  *
  identifier(arr)
  )
  ;
  static
  int
  identifier(arr)
  [
  const int(4)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  ,
  const int(4)
  }
  ;
  int
  *
  identifier(ptr)
  =
  identifier(return_one)
  (
  )
  +
  (
  *
  identifier(small_int_ptr)
  )
  +
  (
  identifier(flag)
  ?
  identifier(get_elem1_ptr)
  (
  identifier(arr)
  )
  :
  identifier(get_elem2_ptr)
  (
  identifier(arr)
  )
  )
  ;
  return
  (
  identifier(ptr)
  ==
  identifier(arr)
  +
  const int(1)
  &&
  *
  identifier(ptr)
  ==
  const int(2)
  )
  ;
  }
  int
  identifier(return_one)
  (
  void
  )
  {
  return
  const int(1)
  ;
  }
  int
  *
  identifier(get_elem1_ptr)
  (
  int
  *
  identifier(arr)
  )
  {
  return
  identifier(arr)
  +
  const int(1)
  ;
  }
  int
  *
  identifier(get_elem2_ptr)
  (
  int
  *
  identifier(arr)
  )
  {
  return
  identifier(arr)
  +
  const int(2)
  ;
  }
  int
  identifier(test_add_multi_dimensional)
  (
  void
  )
  {
  static
  int
  identifier(index)
  =
  const int(2)
  ;
  int
  identifier(nested_arr)
  [
  const int(3)
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
  ,
  {
  const int(7)
  ,
  const int(8)
  ,
  const int(9)
  }
  }
  ;
  int
  (
  *
  identifier(row_pointer)
  )
  [
  const int(3)
  ]
  =
  identifier(nested_arr)
  +
  identifier(index)
  ;
  return
  *
  *
  identifier(row_pointer)
  ==
  const int(7)
  ;
  }
  int
  identifier(test_add_to_subarray_pointer)
  (
  void
  )
  {
  static
  int
  identifier(index)
  =
  const int(2)
  ;
  int
  identifier(nested_arr)
  [
  const int(3)
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
  ,
  {
  const int(7)
  ,
  const int(8)
  ,
  const int(9)
  }
  }
  ;
  int
  *
  identifier(row1)
  =
  *
  (
  identifier(nested_arr)
  +
  const int(1)
  )
  ;
  int
  *
  identifier(elem_ptr)
  =
  identifier(row1)
  +
  identifier(index)
  ;
  return
  *
  identifier(elem_ptr)
  ==
  const int(6)
  ;
  }
  int
  identifier(test_subtract_from_pointer)
  (
  void
  )
  {
  long
  identifier(long_arr)
  [
  const int(5)
  ]
  =
  {
  const int(10)
  ,
  const int(9)
  ,
  const int(8)
  ,
  const int(7)
  ,
  const int(6)
  }
  ;
  long
  *
  identifier(one_past_the_end)
  =
  identifier(long_arr)
  +
  const int(5)
  ;
  static
  int
  identifier(index)
  =
  const int(3)
  ;
  long
  *
  identifier(subtraction_result)
  =
  identifier(one_past_the_end)
  -
  identifier(index)
  ;
  return
  *
  identifier(subtraction_result)
  ==
  const int(8)
  ;
  }
  int
  identifier(test_subtract_negative_index)
  (
  void
  )
  {
  unsigned
  identifier(arr)
  [
  const int(5)
  ]
  =
  {
  const int(100)
  ,
  const int(101)
  ,
  const int(102)
  ,
  const int(103)
  ,
  const int(104)
  }
  ;
  unsigned
  *
  identifier(ptr)
  =
  identifier(arr)
  -
  (
  -
  const int(3)
  )
  ;
  return
  *
  identifier(ptr)
  ==
  const int(103)
  ;
  }
  int
  identifier(test_subtract_different_index_types)
  (
  void
  )
  {
  double
  identifier(double_arr)
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
  const double(6.0)
  }
  ;
  double
  *
  identifier(end_ptr)
  =
  identifier(double_arr)
  +
  const int(11)
  ;
  double
  *
  identifier(ptr1)
  =
  identifier(end_ptr)
  -
  const int(5)
  ;
  double
  *
  identifier(ptr2)
  =
  identifier(end_ptr)
  -
  const long(5l)
  ;
  double
  *
  identifier(ptr3)
  =
  identifier(end_ptr)
  -
  const unsigned int(5u)
  ;
  double
  *
  identifier(ptr4)
  =
  identifier(end_ptr)
  -
  const unsigned long(5ul)
  ;
  return
  (
  identifier(ptr1)
  ==
  identifier(ptr2)
  &&
  identifier(ptr1)
  ==
  identifier(ptr3)
  &&
  identifier(ptr1)
  ==
  identifier(ptr4)
  &&
  *
  identifier(ptr4)
  ==
  const double(6.0)
  )
  ;
  }
  int
  identifier(test_subtract_complex_expressions)
  (
  void
  )
  {
  static
  int
  identifier(flag)
  =
  const int(1)
  ;
  static
  int
  identifier(four)
  =
  const int(4)
  ;
  static
  int
  identifier(arr)
  [
  const int(4)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  ,
  const int(4)
  }
  ;
  int
  *
  identifier(ptr)
  =
  (
  identifier(flag)
  ?
  identifier(get_elem1_ptr)
  (
  identifier(arr)
  )
  :
  identifier(get_elem2_ptr)
  (
  identifier(arr)
  )
  )
  -
  (
  identifier(four)
  /
  -
  const int(2)
  )
  ;
  return
  (
  *
  identifier(ptr)
  ==
  const int(4)
  )
  ;
  }
  int
  identifier(test_subtract_multi_dimensional)
  (
  void
  )
  {
  static
  int
  identifier(index)
  =
  const int(1)
  ;
  int
  identifier(nested_arr)
  [
  const int(3)
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
  ,
  {
  const int(7)
  ,
  const int(8)
  ,
  const int(9)
  }
  }
  ;
  int
  (
  *
  identifier(last_row_pointer)
  )
  [
  const int(3)
  ]
  =
  identifier(nested_arr)
  +
  const int(2)
  ;
  int
  (
  *
  identifier(row_pointer)
  )
  [
  const int(3)
  ]
  =
  identifier(last_row_pointer)
  -
  identifier(index)
  ;
  return
  (
  *
  *
  identifier(row_pointer)
  ==
  const int(4)
  )
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  !
  identifier(test_add_constant_to_pointer)
  (
  )
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  !
  identifier(test_add_negative_index)
  (
  )
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  !
  identifier(test_add_pointer_to_int)
  (
  )
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  !
  identifier(test_add_different_index_types)
  (
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
  identifier(test_add_complex_expressions)
  (
  )
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  !
  identifier(test_add_multi_dimensional)
  (
  )
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  !
  identifier(test_add_to_subarray_pointer)
  (
  )
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  !
  identifier(test_subtract_from_pointer)
  (
  )
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  !
  identifier(test_subtract_negative_index)
  (
  )
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  !
  identifier(test_subtract_different_index_types)
  (
  )
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  !
  identifier(test_subtract_complex_expressions)
  (
  )
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
