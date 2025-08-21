-- Lexing ... OK
+
+
@@ Tokens @@
List[605]:
  void
  identifier(exit)
  (
  int
  identifier(status)
  )
  ;
  struct
  identifier(chars)
  {
  char
  identifier(char_array)
  [
  const int(3)
  ]
  ;
  }
  ;
  static
  struct
  identifier(chars)
  identifier(y)
  =
  {
  {
  const int(0)
  ,
  const int(1)
  ,
  const int(2)
  }
  }
  ;
  static
  struct
  identifier(chars)
  *
  identifier(ptr)
  ;
  void
  identifier(validate_array)
  (
  char
  *
  identifier(char_array)
  ,
  int
  identifier(start)
  ,
  int
  identifier(code)
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
  identifier(char_array)
  [
  identifier(i)
  ]
  !=
  identifier(start)
  +
  identifier(i)
  )
  {
  identifier(exit)
  (
  identifier(code)
  )
  ;
  }
  }
  return
  ;
  }
  void
  identifier(increment_y)
  (
  void
  )
  {
  identifier(y)
  .
  identifier(char_array)
  [
  const int(0)
  ]
  =
  identifier(y)
  .
  identifier(char_array)
  [
  const int(0)
  ]
  +
  const int(3)
  ;
  identifier(y)
  .
  identifier(char_array)
  [
  const int(1)
  ]
  =
  identifier(y)
  .
  identifier(char_array)
  [
  const int(1)
  ]
  +
  const int(3)
  ;
  identifier(y)
  .
  identifier(char_array)
  [
  const int(2)
  ]
  =
  identifier(y)
  .
  identifier(char_array)
  [
  const int(2)
  ]
  +
  const int(3)
  ;
  }
  int
  identifier(test_copy)
  (
  void
  )
  {
  struct
  identifier(chars)
  identifier(a)
  =
  {
  string literal("abc")
  }
  ;
  struct
  identifier(chars)
  identifier(b)
  =
  {
  string literal("xyz")
  }
  ;
  struct
  identifier(chars)
  identifier(c)
  =
  {
  string literal("def")
  }
  ;
  identifier(b)
  =
  identifier(y)
  ;
  identifier(validate_array)
  (
  identifier(a)
  .
  identifier(char_array)
  ,
  const char('a')
  ,
  const int(1)
  )
  ;
  identifier(validate_array)
  (
  identifier(b)
  .
  identifier(char_array)
  ,
  const int(0)
  ,
  const int(2)
  )
  ;
  identifier(validate_array)
  (
  identifier(c)
  .
  identifier(char_array)
  ,
  const char('d')
  ,
  const int(3)
  )
  ;
  return
  const int(0)
  ;
  }
  static
  struct
  identifier(chars)
  identifier(to_validate)
  ;
  void
  identifier(validate_static)
  (
  int
  identifier(start)
  ,
  int
  identifier(code)
  )
  {
  identifier(validate_array)
  (
  identifier(to_validate)
  .
  identifier(char_array)
  ,
  identifier(start)
  ,
  identifier(code)
  )
  ;
  }
  int
  identifier(test_load)
  (
  void
  )
  {
  static
  struct
  identifier(chars)
  identifier(b)
  ;
  struct
  identifier(chars)
  identifier(a)
  =
  {
  string literal("ghi")
  }
  ;
  identifier(b)
  =
  *
  identifier(ptr)
  ;
  identifier(to_validate)
  =
  identifier(a)
  ;
  identifier(validate_static)
  (
  const char('g')
  ,
  const int(4)
  )
  ;
  identifier(to_validate)
  =
  identifier(b)
  ;
  identifier(validate_static)
  (
  const int(3)
  ,
  const int(5)
  )
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(test_store)
  (
  void
  )
  {
  struct
  identifier(chars)
  identifier(struct_array)
  [
  const int(3)
  ]
  =
  {
  {
  string literal("jkl")
  }
  ,
  {
  string literal("xyz")
  }
  ,
  {
  string literal("mno")
  }
  }
  ;
  struct
  identifier(chars)
  *
  identifier(ptr)
  =
  &
  identifier(struct_array)
  [
  const int(1)
  ]
  ;
  *
  identifier(ptr)
  =
  identifier(y)
  ;
  identifier(validate_array)
  (
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(char_array)
  ,
  const char('j')
  ,
  const int(6)
  )
  ;
  identifier(validate_array)
  (
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(char_array)
  ,
  const int(6)
  ,
  const int(7)
  )
  ;
  identifier(validate_array)
  (
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(char_array)
  ,
  const char('m')
  ,
  const int(8)
  )
  ;
  return
  const int(0)
  ;
  }
  struct
  identifier(chars_container)
  {
  char
  identifier(c)
  ;
  struct
  identifier(chars)
  identifier(chars)
  ;
  char
  identifier(arr)
  [
  const int(3)
  ]
  ;
  }
  ;
  int
  identifier(test_copy_from_offset)
  (
  void
  )
  {
  struct
  identifier(chars)
  identifier(a)
  =
  {
  string literal("pqr")
  }
  ;
  static
  struct
  identifier(chars)
  identifier(b)
  =
  {
  string literal("xyz")
  }
  ;
  static
  struct
  identifier(chars_container)
  identifier(container)
  =
  {
  const int(100)
  ,
  {
  {
  const int(9)
  ,
  const int(10)
  ,
  const int(11)
  }
  }
  ,
  string literal("123")
  }
  ;
  identifier(b)
  =
  identifier(container)
  .
  identifier(chars)
  ;
  identifier(to_validate)
  =
  identifier(a)
  ;
  identifier(validate_static)
  (
  const char('p')
  ,
  const int(9)
  )
  ;
  identifier(to_validate)
  =
  identifier(b)
  ;
  identifier(validate_static)
  (
  const int(9)
  ,
  const int(10)
  )
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(test_copy_to_offset)
  (
  void
  )
  {
  struct
  identifier(chars_container)
  identifier(container)
  =
  {
  const char('x')
  ,
  {
  {
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  }
  }
  ,
  string literal("stu")
  }
  ;
  identifier(container)
  .
  identifier(chars)
  =
  identifier(y)
  ;
  if
  (
  identifier(container)
  .
  identifier(c)
  !=
  const char('x')
  )
  {
  identifier(exit)
  (
  const int(11)
  )
  ;
  }
  identifier(validate_array)
  (
  identifier(container)
  .
  identifier(chars)
  .
  identifier(char_array)
  ,
  const int(12)
  ,
  const int(12)
  )
  ;
  identifier(validate_array)
  (
  identifier(container)
  .
  identifier(arr)
  ,
  const char('s')
  ,
  const int(13)
  )
  ;
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
  identifier(ptr)
  =
  &
  identifier(y)
  ;
  identifier(test_copy)
  (
  )
  ;
  identifier(increment_y)
  (
  )
  ;
  identifier(test_load)
  (
  )
  ;
  identifier(increment_y)
  (
  )
  ;
  identifier(test_store)
  (
  )
  ;
  identifier(increment_y)
  (
  )
  ;
  identifier(test_copy_from_offset)
  (
  )
  ;
  identifier(increment_y)
  (
  )
  ;
  identifier(test_copy_to_offset)
  (
  )
  ;
  return
  const int(0)
  ;
  }
