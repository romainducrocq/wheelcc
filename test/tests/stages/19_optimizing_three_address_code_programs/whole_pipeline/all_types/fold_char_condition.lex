-- Lexing ... OK
+
+
@@ Tokens @@
List[293]:
  int
  identifier(putchar)
  (
  int
  identifier(c)
  )
  ;
  int
  identifier(target_not_char)
  (
  void
  )
  {
  char
  identifier(x)
  =
  const int(256)
  ;
  return
  !
  identifier(x)
  ;
  }
  int
  identifier(target_not_uchar)
  (
  void
  )
  {
  unsigned
  char
  identifier(x)
  =
  const int(256)
  ;
  return
  !
  identifier(x)
  ;
  }
  int
  identifier(target_not_true_char)
  (
  void
  )
  {
  char
  identifier(x)
  =
  -
  const int(1)
  ;
  return
  !
  identifier(x)
  ;
  }
  int
  identifier(target_and_schar)
  (
  void
  )
  {
  signed
  char
  identifier(c)
  =
  const int(0)
  ;
  return
  identifier(c)
  &&
  identifier(putchar)
  (
  const char('a')
  )
  ;
  }
  int
  identifier(target_and_true_char)
  (
  void
  )
  {
  signed
  char
  identifier(c1)
  =
  const int(44)
  ;
  char
  identifier(c2)
  =
  identifier(c1)
  -
  const int(10)
  ;
  return
  identifier(c1)
  &&
  identifier(c2)
  ;
  }
  int
  identifier(target_or_uchar)
  (
  void
  )
  {
  unsigned
  char
  identifier(u)
  =
  const int(250)
  ;
  return
  identifier(u)
  ||
  identifier(putchar)
  (
  const char('a')
  )
  ;
  }
  int
  identifier(target_or_char)
  (
  void
  )
  {
  char
  identifier(c)
  =
  const int(250)
  ;
  return
  identifier(c)
  ||
  identifier(putchar)
  (
  const char('a')
  )
  ;
  }
  char
  identifier(target_branch_char)
  (
  void
  )
  {
  unsigned
  char
  identifier(u)
  =
  const int(250)
  ;
  identifier(u)
  =
  identifier(u)
  +
  const int(6)
  ;
  if
  (
  identifier(u)
  )
  {
  identifier(putchar)
  (
  const char('a')
  )
  ;
  }
  return
  identifier(u)
  +
  const int(10)
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
  identifier(target_not_char)
  (
  )
  !=
  const int(1)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(target_not_uchar)
  (
  )
  !=
  const int(1)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_not_true_char)
  (
  )
  !=
  const int(0)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_and_schar)
  (
  )
  !=
  const int(0)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(target_and_true_char)
  (
  )
  !=
  const int(1)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(target_or_uchar)
  (
  )
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
  identifier(target_or_char)
  (
  )
  !=
  const int(1)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(target_branch_char)
  (
  )
  !=
  const int(10)
  )
  {
  return
  const int(8)
  ;
  }
  return
  const int(0)
  ;
  }
