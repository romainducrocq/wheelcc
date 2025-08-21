-- Lexing ... OK
+
+
@@ Tokens @@
List[192]:
  struct
  identifier(inner)
  {
  double
  identifier(d)
  ;
  int
  identifier(i)
  ;
  }
  ;
  struct
  identifier(outer)
  {
  struct
  identifier(inner)
  identifier(s)
  ;
  struct
  identifier(inner)
  *
  identifier(ptr)
  ;
  long
  identifier(l)
  ;
  }
  ;
  int
  identifier(modify_simple_struct)
  (
  struct
  identifier(inner)
  identifier(s)
  )
  ;
  int
  identifier(modify_nested_struct)
  (
  struct
  identifier(outer)
  identifier(s)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(inner)
  identifier(s_inner)
  =
  {
  const double(2.0)
  ,
  const int(3)
  }
  ;
  if
  (
  !
  identifier(modify_simple_struct)
  (
  identifier(s_inner)
  )
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(s_inner)
  .
  identifier(d)
  !=
  const double(2.0)
  ||
  identifier(s_inner)
  .
  identifier(i)
  !=
  const int(3)
  )
  {
  return
  const int(2)
  ;
  }
  struct
  identifier(outer)
  identifier(s_o)
  =
  {
  {
  const double(4.0)
  ,
  const int(5)
  }
  ,
  &
  identifier(s_inner)
  ,
  const long(1000l)
  }
  ;
  if
  (
  !
  identifier(modify_nested_struct)
  (
  identifier(s_o)
  )
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(s_o)
  .
  identifier(s)
  .
  identifier(d)
  !=
  const double(4.0)
  ||
  identifier(s_o)
  .
  identifier(s)
  .
  identifier(i)
  !=
  const int(5)
  ||
  identifier(s_o)
  .
  identifier(l)
  !=
  const long(1000l)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(s_o)
  .
  identifier(ptr)
  !=
  &
  identifier(s_inner)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(s_o)
  .
  identifier(ptr)
  ->
  identifier(d)
  !=
  const double(10.0)
  ||
  identifier(s_o)
  .
  identifier(ptr)
  ->
  identifier(i)
  !=
  const int(11)
  )
  {
  return
  const int(6)
  ;
  }
  return
  const int(0)
  ;
  }
