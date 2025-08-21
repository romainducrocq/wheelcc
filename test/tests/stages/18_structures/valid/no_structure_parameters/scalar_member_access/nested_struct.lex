-- Lexing ... OK
+
+
@@ Tokens @@
List[2107]:
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
  struct
  identifier(inner)
  {
  double
  identifier(a)
  ;
  char
  identifier(b)
  ;
  int
  *
  identifier(ptr)
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
  struct
  identifier(inner)
  identifier(in_array)
  [
  const int(4)
  ]
  ;
  int
  identifier(bar)
  ;
  struct
  identifier(inner)
  identifier(in)
  ;
  }
  ;
  int
  identifier(ptr_target)
  ;
  int
  identifier(test_auto_dot)
  (
  void
  )
  {
  struct
  identifier(outer)
  identifier(s)
  ;
  identifier(s)
  .
  identifier(in)
  .
  identifier(a)
  =
  const double(1.0)
  ;
  identifier(s)
  .
  identifier(in)
  .
  identifier(b)
  =
  const int(2)
  ;
  identifier(s)
  .
  identifier(in)
  .
  identifier(ptr)
  =
  &
  identifier(ptr_target)
  ;
  if
  (
  identifier(s)
  .
  identifier(in)
  .
  identifier(a)
  !=
  const double(1.0)
  ||
  identifier(s)
  .
  identifier(in)
  .
  identifier(b)
  !=
  const int(2)
  ||
  identifier(s)
  .
  identifier(in)
  .
  identifier(ptr)
  !=
  &
  identifier(ptr_target)
  )
  {
  return
  const int(0)
  ;
  }
  char
  *
  identifier(char_ptr)
  =
  &
  identifier(s)
  .
  identifier(in)
  .
  identifier(b)
  ;
  if
  (
  *
  identifier(char_ptr)
  !=
  const int(2)
  )
  {
  return
  const int(0)
  ;
  }
  *
  identifier(s)
  .
  identifier(in)
  .
  identifier(ptr)
  =
  const int(5)
  ;
  if
  (
  identifier(ptr_target)
  !=
  const int(5)
  )
  {
  return
  const int(0)
  ;
  }
  identifier(s)
  .
  identifier(in)
  .
  identifier(a)
  =
  identifier(s)
  .
  identifier(in)
  .
  identifier(b)
  ;
  if
  (
  identifier(s)
  .
  identifier(in)
  .
  identifier(a)
  !=
  const double(2.0)
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
  struct
  identifier(outer)
  identifier(s)
  ;
  identifier(s)
  .
  identifier(in)
  .
  identifier(a)
  =
  const double(1.0)
  ;
  identifier(s)
  .
  identifier(in)
  .
  identifier(b)
  =
  const int(2)
  ;
  identifier(s)
  .
  identifier(in)
  .
  identifier(ptr)
  =
  &
  identifier(ptr_target)
  ;
  if
  (
  identifier(s)
  .
  identifier(in)
  .
  identifier(a)
  !=
  const double(1.0)
  ||
  identifier(s)
  .
  identifier(in)
  .
  identifier(b)
  !=
  const int(2)
  ||
  identifier(s)
  .
  identifier(in)
  .
  identifier(ptr)
  !=
  &
  identifier(ptr_target)
  )
  {
  return
  const int(0)
  ;
  }
  char
  *
  identifier(char_ptr)
  =
  &
  identifier(s)
  .
  identifier(in)
  .
  identifier(b)
  ;
  if
  (
  *
  identifier(char_ptr)
  !=
  const int(2)
  )
  {
  return
  const int(0)
  ;
  }
  *
  identifier(s)
  .
  identifier(in)
  .
  identifier(ptr)
  =
  const int(6)
  ;
  if
  (
  identifier(ptr_target)
  !=
  const int(6)
  )
  {
  return
  const int(0)
  ;
  }
  identifier(s)
  .
  identifier(in)
  .
  identifier(a)
  =
  identifier(s)
  .
  identifier(in)
  .
  identifier(b)
  ;
  if
  (
  identifier(s)
  .
  identifier(in)
  .
  identifier(a)
  !=
  const double(2.0)
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
  struct
  identifier(inner)
  identifier(in)
  ;
  struct
  identifier(outer)
  identifier(s)
  ;
  struct
  identifier(outer)
  *
  identifier(s_ptr)
  =
  &
  identifier(s)
  ;
  identifier(s_ptr)
  ->
  identifier(in_ptr)
  =
  &
  identifier(in)
  ;
  identifier(s_ptr)
  ->
  identifier(l)
  =
  const unsigned long(4294967295ul)
  ;
  identifier(s_ptr)
  ->
  identifier(bar)
  =
  -
  const int(5)
  ;
  identifier(s_ptr)
  ->
  identifier(in_ptr)
  ->
  identifier(a)
  =
  const double(10.0)
  ;
  identifier(s_ptr)
  ->
  identifier(in_ptr)
  ->
  identifier(b)
  =
  const char('x')
  ;
  identifier(s_ptr)
  ->
  identifier(in_array)
  ->
  identifier(a)
  =
  const double(11.0)
  ;
  (
  identifier(s_ptr)
  ->
  identifier(in_array)
  +
  const int(3)
  )
  ->
  identifier(a)
  =
  const double(12.0)
  ;
  identifier(s_ptr)
  ->
  identifier(in_array)
  ->
  identifier(ptr)
  =
  &
  identifier(s_ptr)
  ->
  identifier(bar)
  ;
  if
  (
  identifier(s_ptr)
  ->
  identifier(l)
  !=
  const unsigned long(4294967295ul)
  ||
  identifier(s_ptr)
  ->
  identifier(bar)
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
  identifier(s_ptr)
  ->
  identifier(in_ptr)
  ->
  identifier(a)
  !=
  const double(10.0)
  ||
  identifier(s_ptr)
  ->
  identifier(in_ptr)
  ->
  identifier(b)
  !=
  const char('x')
  ||
  identifier(s_ptr)
  ->
  identifier(in_array)
  ->
  identifier(a)
  !=
  const double(11.0)
  ||
  (
  identifier(s_ptr)
  ->
  identifier(in_array)
  +
  const int(3)
  )
  ->
  identifier(a)
  !=
  const double(12.0)
  )
  {
  return
  const int(0)
  ;
  }
  char
  *
  identifier(char_ptr)
  =
  &
  identifier(s_ptr)
  ->
  identifier(in_ptr)
  ->
  identifier(b)
  ;
  if
  (
  *
  identifier(char_ptr)
  !=
  const char('x')
  )
  {
  return
  const int(0)
  ;
  }
  *
  identifier(s_ptr)
  ->
  identifier(in_array)
  ->
  identifier(ptr)
  =
  const int(123)
  ;
  if
  (
  identifier(s_ptr)
  ->
  identifier(bar)
  !=
  const int(123)
  )
  {
  return
  const int(0)
  ;
  }
  identifier(s_ptr)
  ->
  identifier(in_array)
  ->
  identifier(b)
  =
  identifier(s_ptr)
  ->
  identifier(in_ptr)
  ->
  identifier(b)
  ;
  if
  (
  identifier(s_ptr)
  ->
  identifier(in_array)
  [
  const int(0)
  ]
  .
  identifier(b)
  !=
  const char('x')
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
  struct
  identifier(inner)
  identifier(in)
  ;
  static
  struct
  identifier(outer)
  identifier(s)
  ;
  static
  struct
  identifier(outer)
  *
  identifier(s_ptr)
  ;
  identifier(s_ptr)
  =
  &
  identifier(s)
  ;
  identifier(s_ptr)
  ->
  identifier(in_ptr)
  =
  &
  identifier(in)
  ;
  identifier(s_ptr)
  ->
  identifier(l)
  =
  const unsigned long(4294967295ul)
  ;
  identifier(s_ptr)
  ->
  identifier(bar)
  =
  -
  const int(5)
  ;
  identifier(s_ptr)
  ->
  identifier(in_ptr)
  ->
  identifier(a)
  =
  const double(10.0)
  ;
  identifier(s_ptr)
  ->
  identifier(in_ptr)
  ->
  identifier(b)
  =
  const char('x')
  ;
  identifier(s_ptr)
  ->
  identifier(in_array)
  ->
  identifier(a)
  =
  const double(11.0)
  ;
  (
  identifier(s_ptr)
  ->
  identifier(in_array)
  +
  const int(3)
  )
  ->
  identifier(a)
  =
  const double(12.0)
  ;
  identifier(s_ptr)
  ->
  identifier(in_array)
  ->
  identifier(ptr)
  =
  &
  identifier(s_ptr)
  ->
  identifier(bar)
  ;
  if
  (
  identifier(s_ptr)
  ->
  identifier(l)
  !=
  const unsigned long(4294967295ul)
  ||
  identifier(s_ptr)
  ->
  identifier(bar)
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
  identifier(s_ptr)
  ->
  identifier(in_ptr)
  ->
  identifier(a)
  !=
  const double(10.0)
  ||
  identifier(s_ptr)
  ->
  identifier(in_ptr)
  ->
  identifier(b)
  !=
  const char('x')
  ||
  identifier(s_ptr)
  ->
  identifier(in_array)
  ->
  identifier(a)
  !=
  const double(11.0)
  ||
  (
  identifier(s_ptr)
  ->
  identifier(in_array)
  +
  const int(3)
  )
  ->
  identifier(a)
  !=
  const double(12.0)
  )
  {
  return
  const int(0)
  ;
  }
  char
  *
  identifier(char_ptr)
  =
  &
  identifier(s_ptr)
  ->
  identifier(in_ptr)
  ->
  identifier(b)
  ;
  if
  (
  *
  identifier(char_ptr)
  !=
  const char('x')
  )
  {
  return
  const int(0)
  ;
  }
  *
  identifier(s_ptr)
  ->
  identifier(in_array)
  ->
  identifier(ptr)
  =
  const int(123)
  ;
  if
  (
  identifier(s_ptr)
  ->
  identifier(bar)
  !=
  const int(123)
  )
  {
  return
  const int(0)
  ;
  }
  identifier(s_ptr)
  ->
  identifier(in_ptr)
  ->
  identifier(b)
  =
  identifier(s_ptr)
  ->
  identifier(in_ptr)
  ->
  identifier(a)
  ;
  if
  (
  identifier(s_ptr)
  ->
  identifier(in_ptr)
  ->
  identifier(b)
  !=
  const int(10)
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
  identifier(test_mixed)
  (
  void
  )
  {
  struct
  identifier(inner)
  *
  identifier(in_ptr)
  =
  identifier(malloc)
  (
  sizeof
  (
  struct
  identifier(inner)
  )
  )
  ;
  struct
  identifier(outer)
  identifier(out)
  ;
  identifier(out)
  .
  identifier(in_ptr)
  =
  identifier(in_ptr)
  ;
  struct
  identifier(outer)
  *
  identifier(out_ptr)
  =
  &
  identifier(out)
  ;
  identifier(out)
  .
  identifier(l)
  =
  const int(10)
  ;
  identifier(out)
  .
  identifier(bar)
  =
  const int(20)
  ;
  identifier(out)
  .
  identifier(in_ptr)
  ->
  identifier(a)
  =
  -
  const double(1.0)
  ;
  identifier(out)
  .
  identifier(in_ptr)
  ->
  identifier(b)
  =
  const char('!')
  ;
  identifier(out)
  .
  identifier(in_ptr)
  ->
  identifier(ptr)
  =
  const int(0)
  ;
  identifier(out_ptr)
  ->
  identifier(in_array)
  [
  const int(0)
  ]
  .
  identifier(a)
  =
  -
  const double(2.0)
  ;
  identifier(out_ptr)
  ->
  identifier(in_array)
  [
  const int(0)
  ]
  .
  identifier(b)
  =
  const char('?')
  ;
  identifier(out_ptr)
  ->
  identifier(in_array)
  [
  const int(0)
  ]
  .
  identifier(ptr)
  =
  const int(0)
  ;
  identifier(out_ptr)
  ->
  identifier(in_array)
  [
  const int(3)
  ]
  .
  identifier(a)
  =
  -
  const double(3.0)
  ;
  identifier(out_ptr)
  ->
  identifier(in_array)
  [
  const int(3)
  ]
  .
  identifier(b)
  =
  const char('*')
  ;
  identifier(out_ptr)
  ->
  identifier(in_array)
  [
  const int(3)
  ]
  .
  identifier(ptr)
  =
  identifier(malloc)
  (
  sizeof
  (
  int
  )
  )
  ;
  identifier(out_ptr)
  ->
  identifier(in)
  .
  identifier(a)
  =
  -
  const double(3.0)
  ;
  identifier(out_ptr)
  ->
  identifier(in)
  .
  identifier(b)
  =
  const char('&')
  ;
  int
  identifier(i)
  =
  const int(9)
  ;
  identifier(out_ptr)
  ->
  identifier(in)
  .
  identifier(ptr)
  =
  &
  identifier(i)
  ;
  if
  (
  identifier(out)
  .
  identifier(l)
  !=
  const int(10)
  ||
  identifier(out)
  .
  identifier(bar)
  !=
  const int(20)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(out)
  .
  identifier(in_ptr)
  ->
  identifier(a)
  !=
  -
  const double(1.0)
  ||
  identifier(out)
  .
  identifier(in_ptr)
  ->
  identifier(b)
  !=
  const char('!')
  ||
  identifier(out)
  .
  identifier(in_ptr)
  ->
  identifier(ptr)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(out_ptr)
  ->
  identifier(in_array)
  [
  const int(0)
  ]
  .
  identifier(a)
  !=
  -
  const double(2.0)
  ||
  identifier(out_ptr)
  ->
  identifier(in_array)
  [
  const int(0)
  ]
  .
  identifier(b)
  !=
  const char('?')
  ||
  identifier(out_ptr)
  ->
  identifier(in_array)
  [
  const int(0)
  ]
  .
  identifier(ptr)
  ||
  identifier(out_ptr)
  ->
  identifier(in_array)
  [
  const int(3)
  ]
  .
  identifier(a)
  !=
  -
  const double(3.0)
  ||
  identifier(out_ptr)
  ->
  identifier(in_array)
  [
  const int(3)
  ]
  .
  identifier(b)
  !=
  const char('*')
  ||
  identifier(out_ptr)
  ->
  identifier(in)
  .
  identifier(a)
  !=
  -
  const double(3.0)
  ||
  identifier(out_ptr)
  ->
  identifier(in)
  .
  identifier(b)
  !=
  const char('&')
  ||
  identifier(out_ptr)
  ->
  identifier(in)
  .
  identifier(ptr)
  !=
  &
  identifier(i)
  )
  {
  return
  const int(0)
  ;
  }
  *
  identifier(out_ptr)
  ->
  identifier(in_array)
  [
  const int(3)
  ]
  .
  identifier(ptr)
  =
  const int(5)
  ;
  if
  (
  *
  identifier(out_ptr)
  ->
  identifier(in_array)
  [
  const int(3)
  ]
  .
  identifier(ptr)
  !=
  const int(5)
  )
  {
  return
  const int(0)
  ;
  }
  identifier(out_ptr)
  ->
  identifier(in)
  .
  identifier(b)
  =
  identifier(out)
  .
  identifier(in_ptr)
  ->
  identifier(b)
  ;
  if
  (
  identifier(out_ptr)
  ->
  identifier(in)
  .
  identifier(b)
  !=
  identifier(out)
  .
  identifier(in_ptr)
  ->
  identifier(b)
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
  identifier(test_array_of_structs)
  (
  void
  )
  {
  static
  struct
  identifier(outer)
  identifier(struct_array)
  [
  const int(3)
  ]
  ;
  struct
  identifier(inner)
  *
  identifier(in_ptr)
  =
  identifier(malloc)
  (
  sizeof
  (
  struct
  identifier(inner)
  )
  )
  ;
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(in_ptr)
  =
  identifier(in_ptr)
  ;
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(in_ptr)
  =
  identifier(in_ptr)
  ;
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(in_ptr)
  ->
  identifier(a)
  =
  const double(20.0)
  ;
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(in_ptr)
  ->
  identifier(b)
  =
  const char('@')
  ;
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(in_ptr)
  ->
  identifier(ptr)
  =
  const int(0)
  ;
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(in_array)
  [
  const int(1)
  ]
  .
  identifier(a)
  =
  const double(30.0)
  ;
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(in_array)
  [
  const int(0)
  ]
  .
  identifier(b)
  =
  const char('#')
  ;
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(in)
  .
  identifier(a)
  =
  const double(40.0)
  ;
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(in)
  .
  identifier(b)
  =
  const char('$')
  ;
  if
  (
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(in_ptr)
  ->
  identifier(a)
  !=
  const double(20.0)
  ||
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(in_ptr)
  ->
  identifier(b)
  !=
  const char('@')
  ||
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(in_ptr)
  ->
  identifier(ptr)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(in_array)
  [
  const int(1)
  ]
  .
  identifier(a)
  !=
  const double(30.0)
  ||
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(in_array)
  [
  const int(0)
  ]
  .
  identifier(b)
  !=
  const char('#')
  ||
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(in)
  .
  identifier(a)
  !=
  const double(40.0)
  ||
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(in)
  .
  identifier(b)
  !=
  const char('$')
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
  identifier(test_array_of_struct_pointers)
  (
  void
  )
  {
  struct
  identifier(outer)
  *
  identifier(ptr_array)
  [
  const int(2)
  ]
  ;
  identifier(ptr_array)
  [
  const int(0)
  ]
  =
  identifier(calloc)
  (
  const int(1)
  ,
  sizeof
  (
  struct
  identifier(outer)
  )
  )
  ;
  identifier(ptr_array)
  [
  const int(1)
  ]
  =
  identifier(calloc)
  (
  const int(1)
  ,
  sizeof
  (
  struct
  identifier(outer)
  )
  )
  ;
  identifier(ptr_array)
  [
  const int(1)
  ]
  ->
  identifier(in_ptr)
  =
  identifier(calloc)
  (
  const int(1)
  ,
  sizeof
  (
  struct
  identifier(inner)
  )
  )
  ;
  identifier(ptr_array)
  [
  const int(1)
  ]
  ->
  identifier(in_ptr)
  ->
  identifier(ptr)
  =
  const int(0)
  ;
  identifier(ptr_array)
  [
  const int(1)
  ]
  ->
  identifier(in_ptr)
  ->
  identifier(b)
  =
  const char('%')
  ;
  identifier(ptr_array)
  [
  const int(1)
  ]
  ->
  identifier(in_ptr)
  ->
  identifier(a)
  =
  const double(876.5)
  ;
  identifier(ptr_array)
  [
  const int(1)
  ]
  ->
  identifier(in_array)
  [
  const int(2)
  ]
  .
  identifier(a)
  =
  const double(1000.5)
  ;
  identifier(ptr_array)
  [
  const int(1)
  ]
  ->
  identifier(in)
  .
  identifier(a)
  =
  const double(7e6)
  ;
  identifier(ptr_array)
  [
  const int(0)
  ]
  ->
  identifier(in_ptr)
  =
  identifier(calloc)
  (
  const int(1)
  ,
  sizeof
  (
  struct
  identifier(inner)
  )
  )
  ;
  identifier(ptr_array)
  [
  const int(0)
  ]
  ->
  identifier(in_ptr)
  ->
  identifier(ptr)
  =
  const int(0)
  ;
  identifier(ptr_array)
  [
  const int(0)
  ]
  ->
  identifier(in_ptr)
  ->
  identifier(b)
  =
  const char('^')
  ;
  identifier(ptr_array)
  [
  const int(0)
  ]
  ->
  identifier(in_ptr)
  ->
  identifier(a)
  =
  const double(123.4)
  ;
  identifier(ptr_array)
  [
  const int(0)
  ]
  ->
  identifier(in_array)
  [
  const int(1)
  ]
  .
  identifier(b)
  =
  const char('&')
  ;
  identifier(ptr_array)
  [
  const int(0)
  ]
  ->
  identifier(in)
  .
  identifier(ptr)
  =
  &
  identifier(ptr_array)
  [
  const int(0)
  ]
  ->
  identifier(bar)
  ;
  identifier(ptr_array)
  [
  const int(0)
  ]
  ->
  identifier(bar)
  =
  const int(900)
  ;
  if
  (
  identifier(ptr_array)
  [
  const int(0)
  ]
  ->
  identifier(in_array)
  [
  const int(1)
  ]
  .
  identifier(b)
  !=
  const char('&')
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(ptr_array)
  [
  const int(0)
  ]
  ->
  identifier(in_ptr)
  ->
  identifier(a)
  !=
  const double(123.4)
  ||
  identifier(ptr_array)
  [
  const int(0)
  ]
  ->
  identifier(in_ptr)
  ->
  identifier(b)
  !=
  const char('^')
  ||
  identifier(ptr_array)
  [
  const int(0)
  ]
  ->
  identifier(in_ptr)
  ->
  identifier(ptr)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(ptr_array)
  [
  const int(1)
  ]
  ->
  identifier(in)
  .
  identifier(a)
  !=
  const double(7e6)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(ptr_array)
  [
  const int(1)
  ]
  ->
  identifier(in_array)
  [
  const int(2)
  ]
  .
  identifier(a)
  !=
  const double(1000.5)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(ptr_array)
  [
  const int(1)
  ]
  ->
  identifier(in_ptr)
  ->
  identifier(a)
  !=
  const double(876.5)
  ||
  identifier(ptr_array)
  [
  const int(1)
  ]
  ->
  identifier(in_ptr)
  ->
  identifier(b)
  !=
  const char('%')
  ||
  identifier(ptr_array)
  [
  const int(1)
  ]
  ->
  identifier(in_ptr)
  ->
  identifier(ptr)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  *
  identifier(ptr_array)
  [
  const int(0)
  ]
  ->
  identifier(in)
  .
  identifier(ptr)
  !=
  const int(900)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(ptr_array)
  [
  const int(0)
  ]
  ->
  identifier(l)
  )
  {
  return
  const int(0)
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
  const int(4)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  struct
  identifier(inner)
  *
  identifier(elem_ptr)
  =
  &
  identifier(ptr_array)
  [
  const int(0)
  ]
  ->
  identifier(in_array)
  [
  identifier(i)
  ]
  ;
  if
  (
  identifier(elem_ptr)
  ->
  identifier(a)
  ||
  identifier(elem_ptr)
  ->
  identifier(ptr)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(elem_ptr)
  ->
  identifier(b)
  &&
  identifier(i)
  !=
  const int(1)
  )
  {
  return
  const int(0)
  ;
  }
  }
  if
  (
  identifier(ptr_array)
  [
  const int(0)
  ]
  ->
  identifier(in)
  .
  identifier(a)
  ||
  identifier(ptr_array)
  [
  const int(0)
  ]
  ->
  identifier(in)
  .
  identifier(b)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(ptr_array)
  [
  const int(1)
  ]
  ->
  identifier(l)
  ||
  identifier(ptr_array)
  [
  const int(1)
  ]
  ->
  identifier(bar)
  )
  {
  return
  const int(0)
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
  const int(4)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  struct
  identifier(inner)
  *
  identifier(elem_ptr)
  =
  &
  identifier(ptr_array)
  [
  const int(1)
  ]
  ->
  identifier(in_array)
  [
  identifier(i)
  ]
  ;
  if
  (
  identifier(elem_ptr)
  ->
  identifier(b)
  ||
  identifier(elem_ptr)
  ->
  identifier(ptr)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(elem_ptr)
  ->
  identifier(a)
  &&
  identifier(i)
  !=
  const int(2)
  )
  {
  return
  const int(0)
  ;
  }
  }
  if
  (
  identifier(ptr_array)
  [
  const int(1)
  ]
  ->
  identifier(in)
  .
  identifier(b)
  ||
  identifier(ptr_array)
  [
  const int(1)
  ]
  ->
  identifier(in)
  .
  identifier(ptr)
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
  identifier(test_mixed)
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
  identifier(test_array_of_structs)
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
  identifier(test_array_of_struct_pointers)
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
