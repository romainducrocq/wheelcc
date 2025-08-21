-- Lexing ... OK
+
+
@@ Tokens @@
List[99]:
  void
  *
  identifier(malloc)
  (
  unsigned
  long
  identifier(size)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(s)
  {
  int
  identifier(a)
  ;
  int
  identifier(b)
  ;
  }
  ;
  struct
  identifier(s)
  identifier(my_struct)
  =
  {
  const int(12)
  ,
  const int(13)
  }
  ;
  {
  union
  identifier(u)
  ;
  union
  identifier(u)
  *
  identifier(ptr)
  =
  identifier(malloc)
  (
  const int(4)
  )
  ;
  union
  identifier(u)
  {
  int
  identifier(i)
  ;
  unsigned
  int
  identifier(u)
  ;
  }
  ;
  identifier(ptr)
  ->
  identifier(i)
  =
  const int(10)
  ;
  if
  (
  identifier(ptr)
  ->
  identifier(u)
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
  identifier(my_struct)
  .
  identifier(b)
  !=
  const int(13)
  )
  {
  return
  const int(2)
  ;
  }
  }
  return
  const int(0)
  ;
  }
