-- Lexing ... OK
+
+
@@ Tokens @@
List[981]:
  int
  identifier(strcmp)
  (
  char
  *
  identifier(s1)
  ,
  char
  *
  identifier(s2)
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
  struct
  identifier(small)
  {
  int
  identifier(a)
  ;
  long
  identifier(b)
  ;
  }
  ;
  struct
  identifier(s)
  {
  char
  identifier(arr)
  [
  const int(3)
  ]
  ;
  struct
  identifier(small)
  identifier(inner)
  ;
  }
  ;
  struct
  identifier(with_end_padding)
  {
  int
  identifier(a)
  ;
  int
  identifier(b)
  ;
  char
  identifier(c)
  ;
  }
  ;
  int
  identifier(test_copy_to_pointer)
  (
  void
  )
  {
  struct
  identifier(s)
  identifier(y)
  =
  {
  string literal("!?")
  ,
  {
  -
  const int(20)
  ,
  -
  const int(30)
  }
  }
  ;
  struct
  identifier(s)
  *
  identifier(x)
  =
  identifier(malloc)
  (
  sizeof
  (
  struct
  identifier(s)
  )
  )
  ;
  *
  identifier(x)
  =
  identifier(y)
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(x)
  ->
  identifier(arr)
  ,
  string literal("!?")
  )
  ||
  identifier(x)
  ->
  identifier(inner)
  .
  identifier(a)
  !=
  -
  const int(20)
  ||
  identifier(x)
  ->
  identifier(inner)
  .
  identifier(b)
  !=
  -
  const int(30)
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
  identifier(test_copy_from_pointer)
  (
  void
  )
  {
  static
  struct
  identifier(s)
  identifier(my_struct)
  =
  {
  string literal("()")
  ,
  {
  const int(77)
  ,
  const int(78)
  }
  }
  ;
  struct
  identifier(s)
  *
  identifier(y)
  =
  &
  identifier(my_struct)
  ;
  struct
  identifier(s)
  identifier(x)
  =
  {
  string literal("")
  ,
  {
  const int(0)
  ,
  const int(0)
  }
  }
  ;
  identifier(x)
  =
  *
  identifier(y)
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(x)
  .
  identifier(arr)
  ,
  string literal("()")
  )
  ||
  identifier(x)
  .
  identifier(inner)
  .
  identifier(a)
  !=
  const int(77)
  ||
  identifier(x)
  .
  identifier(inner)
  .
  identifier(b)
  !=
  const int(78)
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
  identifier(test_copy_to_and_from_pointer)
  (
  void
  )
  {
  struct
  identifier(s)
  identifier(my_struct)
  =
  {
  string literal("+-")
  ,
  {
  const int(1000)
  ,
  const int(1001)
  }
  }
  ;
  struct
  identifier(s)
  *
  identifier(y)
  =
  &
  identifier(my_struct)
  ;
  struct
  identifier(s)
  *
  identifier(x)
  =
  identifier(malloc)
  (
  sizeof
  (
  struct
  identifier(s)
  )
  )
  ;
  *
  identifier(x)
  =
  *
  identifier(y)
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(x)
  ->
  identifier(arr)
  ,
  string literal("+-")
  )
  ||
  identifier(x)
  ->
  identifier(inner)
  .
  identifier(a)
  !=
  const int(1000)
  ||
  identifier(x)
  ->
  identifier(inner)
  .
  identifier(b)
  !=
  const int(1001)
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
  identifier(test_copy_to_array_elem)
  (
  void
  )
  {
  struct
  identifier(s)
  identifier(y)
  =
  {
  string literal("\n\t")
  ,
  {
  const int(10000)
  ,
  const int(20000)
  }
  }
  ;
  static
  struct
  identifier(s)
  identifier(arr)
  [
  const int(3)
  ]
  ;
  identifier(arr)
  [
  const int(1)
  ]
  =
  identifier(y)
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(arr)
  [
  const int(1)
  ]
  .
  identifier(arr)
  ,
  string literal("\n\t")
  )
  ||
  identifier(arr)
  [
  const int(1)
  ]
  .
  identifier(inner)
  .
  identifier(a)
  !=
  const int(10000)
  ||
  identifier(arr)
  [
  const int(1)
  ]
  .
  identifier(inner)
  .
  identifier(b)
  !=
  const int(20000)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(arr)
  [
  const int(0)
  ]
  .
  identifier(inner)
  .
  identifier(a)
  ||
  identifier(arr)
  [
  const int(0)
  ]
  .
  identifier(inner)
  .
  identifier(b)
  ||
  identifier(arr)
  [
  const int(2)
  ]
  .
  identifier(arr)
  [
  const int(0)
  ]
  ||
  identifier(arr)
  [
  const int(2)
  ]
  .
  identifier(arr)
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
  identifier(test_copy_from_array_elem)
  (
  void
  )
  {
  struct
  identifier(s)
  identifier(arr)
  [
  const int(3)
  ]
  =
  {
  {
  string literal("ab")
  ,
  {
  -
  const int(3000)
  ,
  -
  const int(4000)
  }
  }
  ,
  {
  string literal("cd")
  ,
  {
  -
  const int(5000)
  ,
  -
  const int(6000)
  }
  }
  ,
  {
  string literal("ef")
  ,
  {
  -
  const int(7000)
  ,
  -
  const int(8000)
  }
  }
  }
  ;
  struct
  identifier(s)
  identifier(x)
  =
  {
  string literal("")
  ,
  {
  const int(0)
  ,
  const int(0)
  }
  }
  ;
  identifier(x)
  =
  identifier(arr)
  [
  const int(1)
  ]
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(x)
  .
  identifier(arr)
  ,
  string literal("cd")
  )
  ||
  identifier(x)
  .
  identifier(inner)
  .
  identifier(a)
  !=
  -
  const int(5000)
  ||
  identifier(x)
  .
  identifier(inner)
  .
  identifier(b)
  !=
  -
  const int(6000)
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
  identifier(test_copy_to_and_from_array_elem)
  (
  void
  )
  {
  struct
  identifier(s)
  identifier(arr)
  [
  const int(3)
  ]
  =
  {
  {
  string literal("ab")
  ,
  {
  -
  const int(3000)
  ,
  -
  const int(4000)
  }
  }
  ,
  {
  string literal("cd")
  ,
  {
  -
  const int(5000)
  ,
  -
  const int(6000)
  }
  }
  ,
  {
  string literal("ef")
  ,
  {
  -
  const int(7000)
  ,
  -
  const int(8000)
  }
  }
  }
  ;
  identifier(arr)
  [
  const int(0)
  ]
  =
  identifier(arr)
  [
  const int(2)
  ]
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(arr)
  [
  const int(0)
  ]
  .
  identifier(arr)
  ,
  string literal("ef")
  )
  ||
  identifier(arr)
  [
  const int(0)
  ]
  .
  identifier(inner)
  .
  identifier(a)
  !=
  -
  const int(7000)
  ||
  identifier(arr)
  [
  const int(0)
  ]
  .
  identifier(inner)
  .
  identifier(b)
  !=
  -
  const int(8000)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(strcmp)
  (
  identifier(arr)
  [
  const int(1)
  ]
  .
  identifier(arr)
  ,
  string literal("cd")
  )
  ||
  identifier(arr)
  [
  const int(1)
  ]
  .
  identifier(inner)
  .
  identifier(a)
  !=
  -
  const int(5000)
  ||
  identifier(arr)
  [
  const int(1)
  ]
  .
  identifier(inner)
  .
  identifier(b)
  !=
  -
  const int(6000)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(strcmp)
  (
  identifier(arr)
  [
  const int(2)
  ]
  .
  identifier(arr)
  ,
  string literal("ef")
  )
  ||
  identifier(arr)
  [
  const int(2)
  ]
  .
  identifier(inner)
  .
  identifier(a)
  !=
  -
  const int(7000)
  ||
  identifier(arr)
  [
  const int(2)
  ]
  .
  identifier(inner)
  .
  identifier(b)
  !=
  -
  const int(8000)
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
  identifier(test_copy_array_element_with_padding)
  (
  void
  )
  {
  struct
  identifier(with_end_padding)
  identifier(arr)
  [
  const int(3)
  ]
  =
  {
  {
  const int(0)
  ,
  const int(1)
  ,
  const int(2)
  }
  ,
  {
  const int(3)
  ,
  const int(4)
  ,
  const int(5)
  }
  ,
  {
  const int(6)
  ,
  const int(7)
  ,
  const int(8)
  }
  }
  ;
  struct
  identifier(with_end_padding)
  identifier(elem)
  =
  {
  const int(9)
  ,
  const int(9)
  ,
  const int(9)
  }
  ;
  identifier(arr)
  [
  const int(1)
  ]
  =
  identifier(elem)
  ;
  if
  (
  identifier(arr)
  [
  const int(0)
  ]
  .
  identifier(a)
  !=
  const int(0)
  ||
  identifier(arr)
  [
  const int(0)
  ]
  .
  identifier(b)
  !=
  const int(1)
  ||
  identifier(arr)
  [
  const int(0)
  ]
  .
  identifier(c)
  !=
  const int(2)
  ||
  identifier(arr)
  [
  const int(1)
  ]
  .
  identifier(a)
  !=
  const int(9)
  ||
  identifier(arr)
  [
  const int(1)
  ]
  .
  identifier(b)
  !=
  const int(9)
  ||
  identifier(arr)
  [
  const int(1)
  ]
  .
  identifier(c)
  !=
  const int(9)
  ||
  identifier(arr)
  [
  const int(2)
  ]
  .
  identifier(a)
  !=
  const int(6)
  ||
  identifier(arr)
  [
  const int(2)
  ]
  .
  identifier(b)
  !=
  const int(7)
  ||
  identifier(arr)
  [
  const int(2)
  ]
  .
  identifier(c)
  !=
  const int(8)
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
  identifier(test_copy_to_pointer)
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
  identifier(test_copy_from_pointer)
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
  identifier(test_copy_to_and_from_pointer)
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
  identifier(test_copy_to_array_elem)
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
  identifier(test_copy_from_array_elem)
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
  identifier(test_copy_to_and_from_array_elem)
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
  identifier(test_copy_array_element_with_padding)
  (
  )
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
