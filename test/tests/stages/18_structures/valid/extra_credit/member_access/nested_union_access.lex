-- Lexing ... OK
+
+
@@ Tokens @@
List[1010]:
  void
  *
  identifier(calloc)
  (
  unsigned
  long
  identifier(nmemb)
  ,
  unsigned
  long
  identifier(size)
  )
  ;
  void
  *
  identifier(malloc)
  (
  unsigned
  long
  identifier(size)
  )
  ;
  union
  identifier(simple)
  {
  int
  identifier(i)
  ;
  long
  identifier(l)
  ;
  char
  identifier(c)
  ;
  unsigned
  char
  identifier(uc_arr)
  [
  const int(3)
  ]
  ;
  }
  ;
  union
  identifier(has_union)
  {
  double
  identifier(d)
  ;
  union
  identifier(simple)
  identifier(u)
  ;
  union
  identifier(simple)
  *
  identifier(u_ptr)
  ;
  }
  ;
  struct
  identifier(simple_struct)
  {
  long
  identifier(l)
  ;
  double
  identifier(d)
  ;
  unsigned
  int
  identifier(u)
  ;
  }
  ;
  union
  identifier(has_struct)
  {
  long
  identifier(l)
  ;
  struct
  identifier(simple_struct)
  identifier(s)
  ;
  }
  ;
  struct
  identifier(struct_with_union)
  {
  union
  identifier(simple)
  identifier(u)
  ;
  unsigned
  long
  identifier(ul)
  ;
  }
  ;
  union
  identifier(complex_union)
  {
  double
  identifier(d_arr)
  [
  const int(2)
  ]
  ;
  struct
  identifier(struct_with_union)
  identifier(s)
  ;
  union
  identifier(has_union)
  *
  identifier(u_ptr)
  ;
  }
  ;
  int
  identifier(test_auto_dot)
  (
  void
  )
  {
  union
  identifier(has_union)
  identifier(x)
  ;
  identifier(x)
  .
  identifier(u)
  .
  identifier(l)
  =
  const unsigned int(200000u)
  ;
  if
  (
  identifier(x)
  .
  identifier(u)
  .
  identifier(i)
  !=
  const int(200000)
  )
  {
  return
  const int(0)
  ;
  }
  union
  identifier(has_struct)
  identifier(y)
  ;
  identifier(y)
  .
  identifier(s)
  .
  identifier(l)
  =
  -
  const long(5555l)
  ;
  identifier(y)
  .
  identifier(s)
  .
  identifier(d)
  =
  const double(10.0)
  ;
  identifier(y)
  .
  identifier(s)
  .
  identifier(u)
  =
  const int(100)
  ;
  if
  (
  identifier(y)
  .
  identifier(l)
  !=
  -
  const long(5555l)
  )
  {
  return
  const int(0)
  ;
  }
  union
  identifier(complex_union)
  identifier(z)
  ;
  identifier(z)
  .
  identifier(s)
  .
  identifier(u)
  .
  identifier(i)
  =
  const int(12345)
  ;
  identifier(z)
  .
  identifier(s)
  .
  identifier(ul)
  =
  const int(0)
  ;
  if
  (
  identifier(z)
  .
  identifier(s)
  .
  identifier(u)
  .
  identifier(c)
  !=
  const int(57)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(z)
  .
  identifier(d_arr)
  [
  const int(1)
  ]
  )
  {
  return
  const int(0)
  ;
  }
  unsigned
  int
  *
  identifier(some_int_ptr)
  =
  &
  identifier(y)
  .
  identifier(s)
  .
  identifier(u)
  ;
  union
  identifier(simple)
  *
  identifier(some_union_ptr)
  =
  &
  identifier(z)
  .
  identifier(s)
  .
  identifier(u)
  ;
  if
  (
  *
  identifier(some_int_ptr)
  !=
  const int(100)
  ||
  (
  *
  identifier(some_union_ptr)
  )
  .
  identifier(i)
  !=
  const int(12345)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
  int
  identifier(test_static_dot)
  (
  void
  )
  {
  static
  union
  identifier(has_union)
  identifier(x)
  ;
  identifier(x)
  .
  identifier(u)
  .
  identifier(l)
  =
  const unsigned int(200000u)
  ;
  if
  (
  identifier(x)
  .
  identifier(u)
  .
  identifier(i)
  !=
  const int(200000)
  )
  {
  return
  const int(0)
  ;
  }
  static
  union
  identifier(has_struct)
  identifier(y)
  ;
  identifier(y)
  .
  identifier(s)
  .
  identifier(l)
  =
  -
  const long(5555l)
  ;
  identifier(y)
  .
  identifier(s)
  .
  identifier(d)
  =
  const double(10.0)
  ;
  identifier(y)
  .
  identifier(s)
  .
  identifier(u)
  =
  const int(100)
  ;
  if
  (
  identifier(y)
  .
  identifier(l)
  !=
  -
  const long(5555l)
  )
  {
  return
  const int(0)
  ;
  }
  static
  union
  identifier(complex_union)
  identifier(z)
  ;
  identifier(z)
  .
  identifier(s)
  .
  identifier(u)
  .
  identifier(i)
  =
  const int(12345)
  ;
  identifier(z)
  .
  identifier(s)
  .
  identifier(ul)
  =
  const int(0)
  ;
  if
  (
  identifier(z)
  .
  identifier(s)
  .
  identifier(u)
  .
  identifier(c)
  !=
  const int(57)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(z)
  .
  identifier(d_arr)
  [
  const int(1)
  ]
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
  int
  identifier(test_auto_arrow)
  (
  void
  )
  {
  union
  identifier(simple)
  identifier(inner)
  =
  {
  const int(100)
  }
  ;
  union
  identifier(has_union)
  identifier(outer)
  ;
  union
  identifier(has_union)
  *
  identifier(outer_ptr)
  =
  &
  identifier(outer)
  ;
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  =
  &
  identifier(inner)
  ;
  if
  (
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  ->
  identifier(i)
  !=
  const int(100)
  )
  {
  return
  const int(0)
  ;
  }
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  ->
  identifier(l)
  =
  -
  const int(10)
  ;
  if
  (
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  ->
  identifier(c)
  !=
  -
  const int(10)
  ||
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  ->
  identifier(i)
  !=
  -
  const int(10)
  ||
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  ->
  identifier(l)
  !=
  -
  const int(10)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  ->
  identifier(uc_arr)
  [
  const int(0)
  ]
  !=
  const int(246)
  ||
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  ->
  identifier(uc_arr)
  [
  const int(1)
  ]
  !=
  const int(255)
  ||
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  ->
  identifier(uc_arr)
  [
  const int(2)
  ]
  !=
  const int(255)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
  int
  identifier(test_static_arrow)
  (
  void
  )
  {
  static
  union
  identifier(simple)
  identifier(inner)
  =
  {
  const int(100)
  }
  ;
  static
  union
  identifier(has_union)
  identifier(outer)
  ;
  static
  union
  identifier(has_union)
  *
  identifier(outer_ptr)
  ;
  identifier(outer_ptr)
  =
  &
  identifier(outer)
  ;
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  =
  &
  identifier(inner)
  ;
  if
  (
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  ->
  identifier(i)
  !=
  const int(100)
  )
  {
  return
  const int(0)
  ;
  }
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  ->
  identifier(l)
  =
  -
  const int(10)
  ;
  if
  (
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  ->
  identifier(c)
  !=
  -
  const int(10)
  ||
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  ->
  identifier(i)
  !=
  -
  const int(10)
  ||
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  ->
  identifier(l)
  !=
  -
  const int(10)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  ->
  identifier(uc_arr)
  [
  const int(0)
  ]
  !=
  const int(246)
  ||
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  ->
  identifier(uc_arr)
  [
  const int(1)
  ]
  !=
  const int(255)
  ||
  identifier(outer_ptr)
  ->
  identifier(u_ptr)
  ->
  identifier(uc_arr)
  [
  const int(2)
  ]
  !=
  const int(255)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
  int
  identifier(test_array_of_unions)
  (
  void
  )
  {
  union
  identifier(has_union)
  identifier(arr)
  [
  const int(3)
  ]
  ;
  identifier(arr)
  [
  const int(0)
  ]
  .
  identifier(u)
  .
  identifier(l)
  =
  -
  const int(10000)
  ;
  identifier(arr)
  [
  const int(1)
  ]
  .
  identifier(u)
  .
  identifier(i)
  =
  const int(200)
  ;
  identifier(arr)
  [
  const int(2)
  ]
  .
  identifier(u)
  .
  identifier(c)
  =
  -
  const int(120)
  ;
  if
  (
  identifier(arr)
  [
  const int(0)
  ]
  .
  identifier(u)
  .
  identifier(l)
  !=
  -
  const int(10000)
  ||
  identifier(arr)
  [
  const int(1)
  ]
  .
  identifier(u)
  .
  identifier(c)
  !=
  -
  const int(56)
  ||
  identifier(arr)
  [
  const int(2)
  ]
  .
  identifier(u)
  .
  identifier(uc_arr)
  [
  const int(0)
  ]
  !=
  const int(136)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
  int
  identifier(test_array_of_union_pointers)
  (
  void
  )
  {
  union
  identifier(has_union)
  *
  identifier(ptr_arr)
  [
  const int(3)
  ]
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
  identifier(ptr_arr)
  [
  identifier(i)
  ]
  =
  identifier(calloc)
  (
  const int(1)
  ,
  sizeof
  (
  union
  identifier(has_union)
  )
  )
  ;
  identifier(ptr_arr)
  [
  identifier(i)
  ]
  ->
  identifier(u_ptr)
  =
  identifier(calloc)
  (
  const int(1)
  ,
  sizeof
  (
  union
  identifier(simple)
  )
  )
  ;
  identifier(ptr_arr)
  [
  identifier(i)
  ]
  ->
  identifier(u_ptr)
  ->
  identifier(l)
  =
  identifier(i)
  ;
  }
  if
  (
  identifier(ptr_arr)
  [
  const int(0)
  ]
  ->
  identifier(u_ptr)
  ->
  identifier(l)
  !=
  const int(0)
  ||
  identifier(ptr_arr)
  [
  const int(1)
  ]
  ->
  identifier(u_ptr)
  ->
  identifier(l)
  !=
  const int(1)
  ||
  identifier(ptr_arr)
  [
  const int(2)
  ]
  ->
  identifier(u_ptr)
  ->
  identifier(l)
  !=
  const int(2)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
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
  identifier(test_auto_dot)
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
  identifier(test_static_dot)
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
  identifier(test_auto_arrow)
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
  identifier(test_static_arrow)
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
  identifier(test_array_of_unions)
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
  identifier(test_array_of_union_pointers)
  (
  )
  )
  {
  return
  const int(6)
  ;
  }
  return
  const int(0)
  ;
  }
