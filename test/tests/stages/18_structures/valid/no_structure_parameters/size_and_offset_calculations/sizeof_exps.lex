-- Lexing ... OK
+
+
@@ Tokens @@
List[350]:
  struct
  identifier(eight_bytes)
  {
  int
  identifier(i)
  ;
  char
  identifier(c)
  ;
  }
  ;
  struct
  identifier(two_bytes)
  {
  char
  identifier(arr)
  [
  const int(2)
  ]
  ;
  }
  ;
  struct
  identifier(three_bytes)
  {
  char
  identifier(arr)
  [
  const int(3)
  ]
  ;
  }
  ;
  struct
  identifier(sixteen_bytes)
  {
  struct
  identifier(eight_bytes)
  identifier(eight)
  ;
  struct
  identifier(two_bytes)
  identifier(two)
  ;
  struct
  identifier(three_bytes)
  identifier(three)
  ;
  }
  ;
  struct
  identifier(seven_bytes)
  {
  struct
  identifier(two_bytes)
  identifier(two)
  ;
  struct
  identifier(three_bytes)
  identifier(three)
  ;
  struct
  identifier(two_bytes)
  identifier(two2)
  ;
  }
  ;
  struct
  identifier(twentyfour_bytes)
  {
  struct
  identifier(seven_bytes)
  identifier(seven)
  ;
  struct
  identifier(sixteen_bytes)
  identifier(sixteen)
  ;
  }
  ;
  struct
  identifier(twenty_bytes)
  {
  struct
  identifier(sixteen_bytes)
  identifier(sixteen)
  ;
  struct
  identifier(two_bytes)
  identifier(two)
  ;
  }
  ;
  struct
  identifier(wonky)
  {
  char
  identifier(arr)
  [
  const int(19)
  ]
  ;
  }
  ;
  struct
  identifier(internal_padding)
  {
  char
  identifier(c)
  ;
  double
  identifier(d)
  ;
  }
  ;
  struct
  identifier(contains_struct_array)
  {
  char
  identifier(c)
  ;
  struct
  identifier(eight_bytes)
  identifier(struct_array)
  [
  const int(3)
  ]
  ;
  }
  ;
  struct
  identifier(twenty_bytes)
  *
  identifier(get_twentybyte_ptr)
  (
  void
  )
  {
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
  struct
  identifier(contains_struct_array)
  identifier(arr_struct)
  ;
  if
  (
  sizeof
  identifier(arr_struct)
  .
  identifier(struct_array)
  [
  const int(2)
  ]
  !=
  const int(8)
  )
  {
  return
  const int(1)
  ;
  }
  static
  struct
  identifier(twentyfour_bytes)
  identifier(twentyfour)
  ;
  if
  (
  sizeof
  identifier(twentyfour)
  .
  identifier(seven)
  .
  identifier(two2)
  !=
  const int(2)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  sizeof
  identifier(get_twentybyte_ptr)
  (
  )
  ->
  identifier(sixteen)
  .
  identifier(three)
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
  sizeof
  identifier(get_twentybyte_ptr)
  (
  )
  ->
  identifier(sixteen)
  !=
  const int(16)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  sizeof
  identifier(twentyfour)
  .
  identifier(seven)
  !=
  const int(7)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  sizeof
  identifier(twentyfour)
  !=
  const int(24)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  sizeof
  *
  identifier(get_twentybyte_ptr)
  (
  )
  !=
  const int(20)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  sizeof
  *
  (
  (
  struct
  identifier(wonky)
  *
  )
  const int(0)
  )
  !=
  const int(19)
  )
  {
  return
  const int(8)
  ;
  }
  extern
  struct
  identifier(internal_padding)
  identifier(struct_array)
  [
  const int(4)
  ]
  ;
  if
  (
  sizeof
  identifier(struct_array)
  [
  const int(0)
  ]
  !=
  const int(16)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  sizeof
  identifier(arr_struct)
  !=
  const int(28)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  sizeof
  identifier(struct_array)
  !=
  const int(64)
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  sizeof
  identifier(arr_struct)
  .
  identifier(struct_array)
  !=
  const int(24)
  )
  {
  return
  const int(12)
  ;
  }
  return
  const int(0)
  ;
  }
