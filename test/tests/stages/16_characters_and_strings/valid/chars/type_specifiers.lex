-- Lexing ... OK
+
+
@@ Tokens @@
List[118]:
  char
  signed
  static
  identifier(a)
  =
  const int(10)
  ;
  unsigned
  static
  char
  identifier(b)
  =
  const int(20)
  ;
  char
  identifier(c)
  =
  const int(30)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  extern
  signed
  char
  identifier(a)
  ;
  char
  unsigned
  extern
  identifier(b)
  ;
  extern
  char
  identifier(c)
  ;
  if
  (
  identifier(a)
  !=
  const int(10)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(b)
  !=
  const int(20)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(c)
  !=
  const int(30)
  )
  {
  return
  const int(3)
  ;
  }
  int
  identifier(loop_counter)
  =
  const int(0)
  ;
  for
  (
  unsigned
  char
  identifier(d)
  =
  const int(0)
  ;
  identifier(d)
  <
  const int(100)
  ;
  identifier(d)
  =
  identifier(d)
  +
  const int(1)
  )
  {
  identifier(loop_counter)
  =
  identifier(loop_counter)
  +
  const int(1)
  ;
  }
  if
  (
  identifier(loop_counter)
  !=
  const int(100)
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
