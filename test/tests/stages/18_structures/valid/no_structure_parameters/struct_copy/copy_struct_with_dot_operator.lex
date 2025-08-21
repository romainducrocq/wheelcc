-- Lexing ... OK
+
+
@@ Tokens @@
List[1099]:
  struct
  identifier(inner)
  {
  signed
  char
  identifier(a)
  ;
  signed
  char
  identifier(b)
  ;
  signed
  char
  identifier(arr)
  [
  const int(3)
  ]
  ;
  }
  ;
  struct
  identifier(outer)
  {
  struct
  identifier(inner)
  identifier(substruct)
  ;
  signed
  char
  identifier(x)
  ;
  signed
  char
  identifier(y)
  ;
  }
  ;
  struct
  identifier(outermost)
  {
  struct
  identifier(outer)
  identifier(nested)
  ;
  int
  identifier(i)
  ;
  }
  ;
  int
  identifier(test_copy_from_member)
  (
  void
  )
  {
  static
  struct
  identifier(outer)
  identifier(big_struct)
  =
  {
  {
  const int(10)
  ,
  const int(9)
  ,
  {
  const int(8)
  ,
  const int(7)
  ,
  const int(6)
  }
  }
  ,
  const int(5)
  ,
  const int(4)
  }
  ;
  char
  identifier(arr)
  [
  const int(3)
  ]
  =
  {
  const char('a')
  ,
  const char('b')
  ,
  const char('c')
  }
  ;
  struct
  identifier(inner)
  identifier(substruct)
  =
  {
  -
  const int(1)
  ,
  -
  const int(1)
  ,
  {
  -
  const int(1)
  ,
  -
  const int(1)
  ,
  -
  const int(1)
  }
  }
  ;
  char
  identifier(arr2)
  [
  const int(3)
  ]
  =
  {
  const char('d')
  ,
  const char('e')
  ,
  const char('f')
  }
  ;
  identifier(substruct)
  =
  identifier(big_struct)
  .
  identifier(substruct)
  ;
  if
  (
  identifier(substruct)
  .
  identifier(a)
  !=
  const int(10)
  ||
  identifier(substruct)
  .
  identifier(b)
  !=
  const int(9)
  ||
  identifier(substruct)
  .
  identifier(arr)
  [
  const int(0)
  ]
  !=
  const int(8)
  ||
  identifier(substruct)
  .
  identifier(arr)
  [
  const int(1)
  ]
  !=
  const int(7)
  ||
  identifier(substruct)
  .
  identifier(arr)
  [
  const int(2)
  ]
  !=
  const int(6)
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
  !=
  const char('a')
  ||
  identifier(arr)
  [
  const int(1)
  ]
  !=
  const char('b')
  ||
  identifier(arr)
  [
  const int(2)
  ]
  !=
  const char('c')
  ||
  identifier(arr2)
  [
  const int(0)
  ]
  !=
  const char('d')
  ||
  identifier(arr2)
  [
  const int(1)
  ]
  !=
  const char('e')
  ||
  identifier(arr2)
  [
  const int(2)
  ]
  !=
  const char('f')
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
  identifier(test_copy_to_member)
  (
  void
  )
  {
  static
  struct
  identifier(outer)
  identifier(big_struct)
  =
  {
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
  const int(0)
  }
  }
  ,
  const int(0)
  ,
  const int(0)
  }
  ;
  struct
  identifier(inner)
  identifier(small_struct)
  =
  {
  -
  const int(1)
  ,
  -
  const int(2)
  ,
  {
  -
  const int(3)
  ,
  -
  const int(4)
  ,
  -
  const int(5)
  }
  }
  ;
  identifier(big_struct)
  .
  identifier(substruct)
  =
  identifier(small_struct)
  ;
  if
  (
  identifier(big_struct)
  .
  identifier(substruct)
  .
  identifier(a)
  !=
  -
  const int(1)
  ||
  identifier(big_struct)
  .
  identifier(substruct)
  .
  identifier(b)
  !=
  -
  const int(2)
  ||
  identifier(big_struct)
  .
  identifier(substruct)
  .
  identifier(arr)
  [
  const int(0)
  ]
  !=
  -
  const int(3)
  ||
  identifier(big_struct)
  .
  identifier(substruct)
  .
  identifier(arr)
  [
  const int(1)
  ]
  !=
  -
  const int(4)
  ||
  identifier(big_struct)
  .
  identifier(substruct)
  .
  identifier(arr)
  [
  const int(2)
  ]
  !=
  -
  const int(5)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(big_struct)
  .
  identifier(x)
  ||
  identifier(big_struct)
  .
  identifier(y)
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
  identifier(test_copy_from_nested_member)
  (
  void
  )
  {
  struct
  identifier(outermost)
  identifier(biggest_struct)
  =
  {
  {
  {
  -
  const int(1)
  ,
  -
  const int(2)
  ,
  {
  -
  const int(3)
  ,
  -
  const int(4)
  ,
  -
  const int(5)
  }
  }
  ,
  -
  const int(6)
  ,
  -
  const int(7)
  }
  ,
  const int(0)
  }
  ;
  static
  struct
  identifier(inner)
  identifier(small_struct)
  ;
  identifier(small_struct)
  =
  identifier(biggest_struct)
  .
  identifier(nested)
  .
  identifier(substruct)
  ;
  if
  (
  identifier(small_struct)
  .
  identifier(a)
  !=
  -
  const int(1)
  ||
  identifier(small_struct)
  .
  identifier(b)
  !=
  -
  const int(2)
  ||
  identifier(small_struct)
  .
  identifier(arr)
  [
  const int(0)
  ]
  !=
  -
  const int(3)
  ||
  identifier(small_struct)
  .
  identifier(arr)
  [
  const int(1)
  ]
  !=
  -
  const int(4)
  ||
  identifier(small_struct)
  .
  identifier(arr)
  [
  const int(2)
  ]
  !=
  -
  const int(5)
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
  identifier(test_copy_to_nested_member)
  (
  void
  )
  {
  struct
  identifier(outermost)
  identifier(biggest_struct)
  =
  {
  {
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
  const int(0)
  }
  }
  ,
  const int(0)
  ,
  const int(0)
  }
  ,
  -
  const int(1)
  }
  ;
  static
  struct
  identifier(inner)
  identifier(small_struct)
  =
  {
  const int(50)
  ,
  const int(51)
  ,
  {
  const int(52)
  ,
  const int(53)
  ,
  const int(54)
  }
  }
  ;
  identifier(biggest_struct)
  .
  identifier(nested)
  .
  identifier(substruct)
  =
  identifier(small_struct)
  ;
  if
  (
  identifier(biggest_struct)
  .
  identifier(nested)
  .
  identifier(substruct)
  .
  identifier(a)
  !=
  const int(50)
  ||
  identifier(biggest_struct)
  .
  identifier(nested)
  .
  identifier(substruct)
  .
  identifier(b)
  !=
  const int(51)
  ||
  identifier(biggest_struct)
  .
  identifier(nested)
  .
  identifier(substruct)
  .
  identifier(arr)
  [
  const int(0)
  ]
  !=
  const int(52)
  ||
  identifier(biggest_struct)
  .
  identifier(nested)
  .
  identifier(substruct)
  .
  identifier(arr)
  [
  const int(1)
  ]
  !=
  const int(53)
  ||
  identifier(biggest_struct)
  .
  identifier(nested)
  .
  identifier(substruct)
  .
  identifier(arr)
  [
  const int(2)
  ]
  !=
  const int(54)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(biggest_struct)
  .
  identifier(nested)
  .
  identifier(x)
  ||
  identifier(biggest_struct)
  .
  identifier(nested)
  .
  identifier(y)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(biggest_struct)
  .
  identifier(i)
  !=
  -
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
  int
  identifier(test_copy_from_conditional)
  (
  void
  )
  {
  struct
  identifier(outer)
  identifier(big_struct)
  =
  {
  {
  const int(127)
  ,
  -
  const int(128)
  ,
  {
  const int(61)
  ,
  const int(62)
  ,
  const int(63)
  }
  }
  ,
  -
  const int(10)
  ,
  -
  const int(11)
  }
  ;
  struct
  identifier(outer)
  identifier(big_struct2)
  =
  {
  {
  const int(0)
  ,
  const int(1)
  ,
  {
  const int(2)
  ,
  const int(3)
  ,
  const int(4)
  }
  }
  ,
  const int(5)
  ,
  const int(6)
  }
  ;
  static
  int
  identifier(t)
  =
  const int(1)
  ;
  static
  int
  identifier(f)
  =
  const int(0)
  ;
  struct
  identifier(inner)
  identifier(small_struct)
  =
  (
  identifier(f)
  ?
  identifier(big_struct)
  :
  identifier(big_struct2)
  )
  .
  identifier(substruct)
  ;
  if
  (
  identifier(small_struct)
  .
  identifier(a)
  !=
  const int(0)
  ||
  identifier(small_struct)
  .
  identifier(b)
  !=
  const int(1)
  ||
  identifier(small_struct)
  .
  identifier(arr)
  [
  const int(0)
  ]
  !=
  const int(2)
  ||
  identifier(small_struct)
  .
  identifier(arr)
  [
  const int(1)
  ]
  !=
  const int(3)
  ||
  identifier(small_struct)
  .
  identifier(arr)
  [
  const int(2)
  ]
  !=
  const int(4)
  )
  {
  return
  const int(0)
  ;
  }
  identifier(small_struct)
  =
  (
  identifier(t)
  ?
  identifier(big_struct)
  :
  identifier(big_struct2)
  )
  .
  identifier(substruct)
  ;
  if
  (
  identifier(small_struct)
  .
  identifier(a)
  !=
  const int(127)
  ||
  identifier(small_struct)
  .
  identifier(b)
  !=
  -
  const int(128)
  ||
  identifier(small_struct)
  .
  identifier(arr)
  [
  const int(0)
  ]
  !=
  const int(61)
  ||
  identifier(small_struct)
  .
  identifier(arr)
  [
  const int(1)
  ]
  !=
  const int(62)
  ||
  identifier(small_struct)
  .
  identifier(arr)
  [
  const int(2)
  ]
  !=
  const int(63)
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
  identifier(test_copy_from_assignment)
  (
  void
  )
  {
  struct
  identifier(outer)
  identifier(big_struct)
  =
  {
  {
  const int(127)
  ,
  -
  const int(128)
  ,
  {
  const int(61)
  ,
  const int(62)
  ,
  const int(63)
  }
  }
  ,
  -
  const int(10)
  ,
  -
  const int(11)
  }
  ;
  static
  struct
  identifier(outer)
  identifier(big_struct2)
  ;
  static
  struct
  identifier(inner)
  identifier(small_struct)
  ;
  identifier(small_struct)
  =
  (
  identifier(big_struct2)
  =
  identifier(big_struct)
  )
  .
  identifier(substruct)
  ;
  if
  (
  identifier(small_struct)
  .
  identifier(a)
  !=
  const int(127)
  ||
  identifier(small_struct)
  .
  identifier(b)
  !=
  -
  const int(128)
  ||
  identifier(small_struct)
  .
  identifier(arr)
  [
  const int(0)
  ]
  !=
  const int(61)
  ||
  identifier(small_struct)
  .
  identifier(arr)
  [
  const int(1)
  ]
  !=
  const int(62)
  ||
  identifier(small_struct)
  .
  identifier(arr)
  [
  const int(2)
  ]
  !=
  const int(63)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(big_struct2)
  .
  identifier(substruct)
  .
  identifier(a)
  !=
  const int(127)
  ||
  identifier(big_struct2)
  .
  identifier(substruct)
  .
  identifier(b)
  !=
  -
  const int(128)
  ||
  identifier(big_struct2)
  .
  identifier(substruct)
  .
  identifier(arr)
  [
  const int(0)
  ]
  !=
  const int(61)
  ||
  identifier(big_struct2)
  .
  identifier(substruct)
  .
  identifier(arr)
  [
  const int(1)
  ]
  !=
  const int(62)
  ||
  identifier(big_struct2)
  .
  identifier(substruct)
  .
  identifier(arr)
  [
  const int(2)
  ]
  !=
  const int(63)
  ||
  identifier(big_struct2)
  .
  identifier(x)
  !=
  -
  const int(10)
  ||
  identifier(big_struct2)
  .
  identifier(y)
  !=
  -
  const int(11)
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
  identifier(test_copy_from_member)
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
  identifier(test_copy_to_member)
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
  identifier(test_copy_from_nested_member)
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
  identifier(test_copy_to_nested_member)
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
  identifier(test_copy_from_conditional)
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
  identifier(test_copy_from_assignment)
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
