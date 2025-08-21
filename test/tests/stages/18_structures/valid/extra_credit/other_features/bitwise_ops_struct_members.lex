-- Lexing ... OK
+
+
@@ Tokens @@
List[176]:
  struct
  identifier(inner)
  {
  char
  identifier(b)
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
  identifier(bar)
  ;
  struct
  identifier(inner)
  identifier(in)
  ;
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(inner)
  identifier(i)
  =
  {
  const char('a')
  ,
  const unsigned int(100000u)
  }
  ;
  struct
  identifier(outer)
  identifier(o)
  =
  {
  const unsigned long(9223372036854775810ul)
  ,
  &
  identifier(i)
  ,
  const int(100)
  ,
  {
  -
  const int(80)
  ,
  const unsigned int(4294967295U)
  }
  }
  ;
  if
  (
  (
  identifier(i)
  .
  identifier(b)
  |
  identifier(o)
  .
  identifier(l)
  )
  !=
  const unsigned long(9223372036854775907ul)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  (
  identifier(o)
  .
  identifier(bar)
  ^
  identifier(i)
  .
  identifier(u)
  )
  !=
  const unsigned int(100036u)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  (
  identifier(o)
  .
  identifier(in_ptr)
  ->
  identifier(b)
  &
  identifier(o)
  .
  identifier(in)
  .
  identifier(b)
  )
  !=
  const int(32)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  (
  identifier(o)
  .
  identifier(l)
  >>
  const int(26)
  )
  !=
  const unsigned long(137438953472ul)
  )
  {
  return
  const int(4)
  ;
  }
  identifier(o)
  .
  identifier(bar)
  =
  const int(12)
  ;
  if
  (
  (
  identifier(i)
  .
  identifier(b)
  <<
  identifier(o)
  .
  identifier(bar)
  )
  !=
  const int(397312)
  )
  {
  return
  const int(5)
  ;
  }
  return
  const int(0)
  ;
  }
