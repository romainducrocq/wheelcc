-- Lexing ... OK
+
+
@@ Tokens @@
List[439]:
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
  identifier(test_auto)
  (
  void
  )
  {
  struct
  identifier(s)
  identifier(x)
  =
  {
  string literal("ab")
  ,
  {
  -
  const int(1)
  ,
  const int(2)
  }
  }
  ;
  struct
  identifier(s)
  identifier(y)
  =
  {
  string literal("x")
  ,
  {
  const int(1)
  }
  }
  ;
  identifier(y)
  =
  identifier(x)
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(y)
  .
  identifier(arr)
  ,
  string literal("ab")
  )
  ||
  identifier(y)
  .
  identifier(inner)
  .
  identifier(a)
  !=
  -
  const int(1)
  ||
  identifier(y)
  .
  identifier(inner)
  .
  identifier(b)
  !=
  const int(2)
  )
  {
  return
  const int(0)
  ;
  }
  identifier(y)
  .
  identifier(inner)
  .
  identifier(a)
  =
  const int(20)
  ;
  if
  (
  identifier(y)
  .
  identifier(inner)
  .
  identifier(a)
  !=
  const int(20)
  ||
  identifier(x)
  .
  identifier(inner)
  .
  identifier(a)
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
  identifier(test_static)
  (
  void
  )
  {
  static
  struct
  identifier(s)
  identifier(x)
  =
  {
  string literal("ab")
  ,
  {
  const int(1)
  ,
  const int(2)
  }
  }
  ;
  static
  struct
  identifier(s)
  identifier(y)
  ;
  identifier(y)
  =
  identifier(x)
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(y)
  .
  identifier(arr)
  ,
  string literal("ab")
  )
  ||
  identifier(y)
  .
  identifier(inner)
  .
  identifier(a)
  !=
  const int(1)
  ||
  identifier(y)
  .
  identifier(inner)
  .
  identifier(b)
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
  struct
  identifier(wonky)
  {
  char
  identifier(arr)
  [
  const int(7)
  ]
  ;
  }
  ;
  int
  identifier(test_wonky_size)
  (
  void
  )
  {
  struct
  identifier(wonky)
  identifier(x)
  =
  {
  string literal("abcdef")
  }
  ;
  static
  struct
  identifier(wonky)
  identifier(y)
  ;
  identifier(y)
  =
  identifier(x)
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(y)
  .
  identifier(arr)
  ,
  string literal("abcdef")
  )
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
  identifier(true_flag)
  (
  void
  )
  {
  return
  const int(1)
  ;
  }
  int
  identifier(test_conditional)
  (
  void
  )
  {
  static
  struct
  identifier(s)
  identifier(x)
  =
  {
  string literal("xy")
  ,
  {
  const int(1234)
  ,
  const int(5678)
  }
  }
  ;
  struct
  identifier(s)
  identifier(y)
  =
  {
  string literal("!")
  ,
  {
  -
  const int(10)
  }
  }
  ;
  struct
  identifier(s)
  identifier(z)
  ;
  identifier(z)
  =
  identifier(true_flag)
  (
  )
  ?
  identifier(x)
  :
  identifier(y)
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(z)
  .
  identifier(arr)
  ,
  string literal("xy")
  )
  ||
  identifier(z)
  .
  identifier(inner)
  .
  identifier(a)
  !=
  const int(1234)
  ||
  identifier(z)
  .
  identifier(inner)
  .
  identifier(b)
  !=
  const int(5678)
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
  identifier(test_auto)
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
  identifier(test_static)
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
  identifier(test_wonky_size)
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
  identifier(test_conditional)
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
