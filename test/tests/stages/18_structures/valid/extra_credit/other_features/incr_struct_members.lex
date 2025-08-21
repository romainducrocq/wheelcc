-- Lexing ... OK
+
+
@@ Tokens @@
List[408]:
  struct
  identifier(inner)
  {
  char
  identifier(c)
  ;
  unsigned
  int
  identifier(u)
  ;
  }
  ;
  struct
  identifier(outer)
  {
  unsigned
  long
  identifier(l)
  ;
  struct
  identifier(inner)
  *
  identifier(in_ptr)
  ;
  int
  identifier(array)
  [
  const int(3)
  ]
  ;
  }
  ;
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
  int
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(outer)
  identifier(my_struct)
  =
  {
  const unsigned long(9223372036854775900ul)
  ,
  identifier(calloc)
  (
  const int(3)
  ,
  sizeof
  (
  struct
  identifier(inner)
  )
  )
  ,
  {
  -
  const int(1000)
  ,
  -
  const int(2000)
  ,
  -
  const int(3000)
  }
  ,
  }
  ;
  struct
  identifier(outer)
  *
  identifier(my_struct_ptr)
  =
  &
  identifier(my_struct)
  ;
  if
  (
  ++
  identifier(my_struct)
  .
  identifier(l)
  !=
  const unsigned long(9223372036854775901ul)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  --
  identifier(my_struct)
  .
  identifier(in_ptr)
  [
  const int(0)
  ]
  .
  identifier(u)
  !=
  const unsigned int(4294967295U)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(my_struct)
  .
  identifier(in_ptr)
  ->
  identifier(c)
  ++
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(my_struct_ptr)
  ->
  identifier(array)
  [
  const int(1)
  ]
  --
  !=
  -
  const int(2000)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(my_struct_ptr)
  ->
  identifier(l)
  !=
  const unsigned long(9223372036854775901ul)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(my_struct)
  .
  identifier(in_ptr)
  ->
  identifier(c)
  !=
  const int(1)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(my_struct_ptr)
  ->
  identifier(in_ptr)
  ->
  identifier(u)
  !=
  const unsigned int(4294967295U)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(my_struct_ptr)
  ->
  identifier(array)
  [
  const int(1)
  ]
  !=
  -
  const int(2001)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(my_struct_ptr)
  ->
  identifier(array)
  [
  const int(0)
  ]
  !=
  -
  const int(1000)
  ||
  identifier(my_struct_ptr)
  ->
  identifier(array)
  [
  const int(2)
  ]
  !=
  -
  const int(3000)
  )
  {
  return
  const int(9)
  ;
  }
  identifier(my_struct_ptr)
  ->
  identifier(in_ptr)
  [
  const int(1)
  ]
  .
  identifier(c)
  =
  -
  const int(1)
  ;
  identifier(my_struct_ptr)
  ->
  identifier(in_ptr)
  [
  const int(1)
  ]
  .
  identifier(u)
  =
  const unsigned int(1u)
  ;
  identifier(my_struct_ptr)
  ->
  identifier(in_ptr)
  [
  const int(2)
  ]
  .
  identifier(c)
  =
  const char('X')
  ;
  identifier(my_struct_ptr)
  ->
  identifier(in_ptr)
  [
  const int(2)
  ]
  .
  identifier(u)
  =
  const unsigned int(100000u)
  ;
  (
  ++
  identifier(my_struct_ptr)
  ->
  identifier(in_ptr)
  )
  ->
  identifier(c)
  --
  ;
  identifier(my_struct_ptr)
  ->
  identifier(in_ptr)
  ++
  ->
  identifier(u)
  ++
  ;
  if
  (
  identifier(my_struct_ptr)
  ->
  identifier(in_ptr)
  [
  -
  const int(2)
  ]
  .
  identifier(c)
  !=
  const int(1)
  ||
  identifier(my_struct_ptr)
  ->
  identifier(in_ptr)
  [
  -
  const int(2)
  ]
  .
  identifier(u)
  !=
  const unsigned int(4294967295U)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  identifier(my_struct_ptr)
  ->
  identifier(in_ptr)
  [
  -
  const int(1)
  ]
  .
  identifier(c)
  !=
  -
  const int(2)
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  identifier(my_struct_ptr)
  ->
  identifier(in_ptr)
  [
  -
  const int(1)
  ]
  .
  identifier(u)
  !=
  const int(2)
  )
  {
  return
  const int(12)
  ;
  }
  if
  (
  identifier(my_struct_ptr)
  ->
  identifier(in_ptr)
  [
  const int(0)
  ]
  .
  identifier(c)
  !=
  const char('X')
  ||
  identifier(my_struct_ptr)
  ->
  identifier(in_ptr)
  [
  const int(0)
  ]
  .
  identifier(u)
  !=
  const unsigned int(100000u)
  )
  {
  return
  const int(13)
  ;
  }
  return
  const int(0)
  ;
  }
