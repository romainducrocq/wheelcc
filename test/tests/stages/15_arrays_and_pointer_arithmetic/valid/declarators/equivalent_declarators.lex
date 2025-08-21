-- Lexing ... OK
+
+
@@ Tokens @@
List[361]:
  long
  int
  (
  identifier(arr)
  )
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
  long
  identifier(arr)
  [
  const unsigned long(4ul)
  ]
  ;
  int
  (
  *
  identifier(ptr_to_arr)
  )
  [
  const int(3)
  ]
  [
  const long(6l)
  ]
  ;
  int
  (
  (
  *
  (
  identifier(ptr_to_arr)
  )
  )
  [
  const long(3l)
  ]
  )
  [
  const unsigned int(6u)
  ]
  =
  const int(0)
  ;
  int
  *
  identifier(array_of_pointers)
  [
  const int(3)
  ]
  =
  {
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  }
  ;
  int
  identifier(test_arr)
  (
  void
  )
  {
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(4)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  if
  (
  identifier(arr)
  [
  identifier(i)
  ]
  !=
  identifier(i)
  +
  const int(1)
  )
  {
  return
  const int(1)
  ;
  }
  }
  return
  const int(0)
  ;
  }
  int
  identifier(test_ptr_to_arr)
  (
  void
  )
  {
  if
  (
  identifier(ptr_to_arr)
  )
  {
  return
  const int(2)
  ;
  }
  static
  int
  identifier(nested_arr)
  [
  const int(3)
  ]
  [
  const int(6)
  ]
  ;
  identifier(ptr_to_arr)
  =
  &
  identifier(nested_arr)
  ;
  identifier(ptr_to_arr)
  [
  const int(0)
  ]
  [
  const int(2)
  ]
  [
  const int(4)
  ]
  =
  const int(100)
  ;
  if
  (
  identifier(nested_arr)
  [
  const int(2)
  ]
  [
  const int(4)
  ]
  !=
  const int(100)
  )
  {
  return
  const int(3)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(test_array_of_pointers)
  (
  int
  *
  identifier(ptr)
  )
  {
  extern
  int
  *
  (
  (
  identifier(array_of_pointers)
  )
  [
  const int(3)
  ]
  )
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
  if
  (
  identifier(array_of_pointers)
  [
  identifier(i)
  ]
  )
  return
  const int(4)
  ;
  identifier(array_of_pointers)
  [
  identifier(i)
  ]
  =
  identifier(ptr)
  ;
  }
  identifier(array_of_pointers)
  [
  const int(2)
  ]
  [
  const int(0)
  ]
  =
  const int(11)
  ;
  if
  (
  *
  identifier(ptr)
  !=
  const int(11)
  )
  {
  return
  const int(5)
  ;
  }
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
  if
  (
  identifier(array_of_pointers)
  [
  identifier(i)
  ]
  [
  const int(0)
  ]
  !=
  const int(11)
  )
  {
  return
  const int(6)
  ;
  }
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
  int
  identifier(check)
  =
  identifier(test_arr)
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
  identifier(check)
  =
  identifier(test_ptr_to_arr)
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
  identifier(x)
  =
  const int(0)
  ;
  identifier(check)
  =
  identifier(test_array_of_pointers)
  (
  &
  identifier(x)
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
