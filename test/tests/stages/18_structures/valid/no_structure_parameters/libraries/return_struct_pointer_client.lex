-- Lexing ... OK
+
+
@@ Tokens @@
List[450]:
  void
  *
  identifier(malloc)
  (
  unsigned
  long
  identifier(size)
  )
  ;
  struct
  identifier(inner)
  {
  double
  identifier(d)
  ;
  int
  identifier(i)
  ;
  }
  ;
  struct
  identifier(outer)
  {
  char
  identifier(a)
  ;
  char
  identifier(b)
  ;
  struct
  identifier(inner)
  identifier(substruct)
  ;
  }
  ;
  struct
  identifier(outermost)
  {
  int
  identifier(i)
  ;
  struct
  identifier(outer)
  *
  identifier(nested_ptr)
  ;
  struct
  identifier(outer)
  identifier(nested_struct)
  ;
  }
  ;
  struct
  identifier(inner)
  *
  identifier(make_struct_inner)
  (
  int
  identifier(seed)
  )
  ;
  struct
  identifier(outer)
  *
  identifier(make_struct_outer)
  (
  int
  identifier(seed)
  )
  ;
  struct
  identifier(outermost)
  *
  identifier(make_struct_outermost)
  (
  int
  identifier(seed)
  )
  ;
  int
  identifier(test_get_struct_ptr)
  (
  void
  )
  {
  struct
  identifier(inner)
  *
  identifier(inner_ptr)
  =
  identifier(make_struct_inner)
  (
  const int(11)
  )
  ;
  if
  (
  identifier(inner_ptr)
  ->
  identifier(d)
  !=
  const int(11)
  ||
  identifier(inner_ptr)
  ->
  identifier(i)
  !=
  const int(11)
  )
  {
  return
  const int(0)
  ;
  }
  struct
  identifier(outermost)
  identifier(o)
  =
  {
  const int(0)
  ,
  const int(0)
  ,
  {
  const int(0)
  ,
  const int(0)
  ,
  {
  const int(0)
  ,
  const int(0)
  }
  }
  }
  ;
  identifier(o)
  .
  identifier(nested_ptr)
  =
  identifier(make_struct_outer)
  (
  const int(20)
  )
  ;
  if
  (
  identifier(o)
  .
  identifier(nested_ptr)
  ->
  identifier(a)
  !=
  const int(20)
  ||
  identifier(o)
  .
  identifier(nested_ptr)
  ->
  identifier(b)
  !=
  const int(21)
  ||
  identifier(o)
  .
  identifier(nested_ptr)
  ->
  identifier(substruct)
  .
  identifier(d)
  !=
  const int(22)
  ||
  identifier(o)
  .
  identifier(nested_ptr)
  ->
  identifier(substruct)
  .
  identifier(i)
  !=
  const int(23)
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
  identifier(test_get_struct_pointer_member)
  (
  void
  )
  {
  if
  (
  identifier(make_struct_inner)
  (
  const int(2)
  )
  ->
  identifier(d)
  !=
  const int(2)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(make_struct_outer)
  (
  const int(2)
  )
  ->
  identifier(substruct)
  .
  identifier(d)
  !=
  const int(4)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(make_struct_outermost)
  (
  const int(0)
  )
  ->
  identifier(nested_ptr)
  ->
  identifier(a)
  !=
  const int(1)
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
  struct
  identifier(outer)
  *
  identifier(get_static_struct_ptr)
  (
  void
  )
  {
  static
  struct
  identifier(outer)
  identifier(s)
  ;
  return
  &
  identifier(s)
  ;
  }
  int
  identifier(test_update_member_thru_retval)
  (
  void
  )
  {
  identifier(get_static_struct_ptr)
  (
  )
  ->
  identifier(a)
  =
  const int(10)
  ;
  identifier(get_static_struct_ptr)
  (
  )
  ->
  identifier(substruct)
  .
  identifier(d)
  =
  const double(20.0)
  ;
  struct
  identifier(outer)
  *
  identifier(ptr)
  =
  identifier(get_static_struct_ptr)
  (
  )
  ;
  if
  (
  identifier(ptr)
  ->
  identifier(a)
  !=
  const int(10)
  ||
  identifier(ptr)
  ->
  identifier(substruct)
  .
  identifier(d)
  !=
  const double(20.0)
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
  identifier(test_update_nested_struct_thru_retval)
  (
  void
  )
  {
  struct
  identifier(inner)
  identifier(small)
  =
  {
  const double(12.0)
  ,
  const int(13)
  }
  ;
  identifier(get_static_struct_ptr)
  (
  )
  ->
  identifier(substruct)
  =
  identifier(small)
  ;
  if
  (
  identifier(get_static_struct_ptr)
  (
  )
  ->
  identifier(substruct)
  .
  identifier(d)
  !=
  const double(12.0)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(get_static_struct_ptr)
  (
  )
  ->
  identifier(substruct)
  .
  identifier(i)
  !=
  const int(13)
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
  identifier(test_get_struct_ptr)
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
  identifier(test_get_struct_pointer_member)
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
  identifier(test_update_member_thru_retval)
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
  identifier(test_update_nested_struct_thru_retval)
  (
  )
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
