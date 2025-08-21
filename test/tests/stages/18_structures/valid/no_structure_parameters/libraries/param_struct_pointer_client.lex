-- Lexing ... OK
+
+
@@ Tokens @@
List[182]:
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
  char
  identifier(a)
  ;
  char
  identifier(b)
  ;
  struct
  identifier(inner)
  identifier(substruct)
  ;
  }
  ;
  int
  identifier(access_members_through_pointer)
  (
  struct
  identifier(outer)
  *
  identifier(ptr)
  ,
  int
  identifier(expected_a)
  ,
  int
  identifier(expected_b)
  ,
  double
  identifier(expected_d)
  ,
  int
  identifier(expected_i)
  )
  ;
  void
  identifier(update_members_through_pointer)
  (
  struct
  identifier(outer)
  *
  identifier(ptr)
  ,
  int
  identifier(a)
  ,
  int
  identifier(b)
  ,
  struct
  identifier(inner)
  *
  identifier(inner_ptr)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(outer)
  identifier(s)
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  {
  const double(3.0)
  ,
  const int(4)
  }
  }
  ;
  if
  (
  !
  identifier(access_members_through_pointer)
  (
  &
  identifier(s)
  ,
  const int(1)
  ,
  const int(2)
  ,
  const double(3.0)
  ,
  const int(4)
  )
  )
  {
  return
  const int(1)
  ;
  }
  struct
  identifier(inner)
  identifier(inner_struct)
  =
  {
  const int(7)
  ,
  const int(8)
  }
  ;
  identifier(update_members_through_pointer)
  (
  &
  identifier(s)
  ,
  const int(5)
  ,
  const int(6)
  ,
  &
  identifier(inner_struct)
  )
  ;
  if
  (
  identifier(s)
  .
  identifier(a)
  !=
  const int(5)
  ||
  identifier(s)
  .
  identifier(b)
  !=
  const int(6)
  ||
  identifier(s)
  .
  identifier(substruct)
  .
  identifier(d)
  !=
  const int(7)
  ||
  identifier(s)
  .
  identifier(substruct)
  .
  identifier(i)
  !=
  const int(8)
  )
  {
  return
  const int(2)
  ;
  }
  return
  const int(0)
  ;
  }
