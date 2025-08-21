-- Lexing ... OK
+
+
@@ Tokens @@
List[122]:
  struct
  identifier(s)
  {
  long
  identifier(l1)
  ;
  long
  identifier(l2)
  ;
  long
  identifier(l3)
  ;
  }
  ;
  extern
  struct
  identifier(s)
  identifier(globvar)
  ;
  struct
  identifier(s)
  identifier(overlap_with_globvar)
  (
  void
  )
  ;
  struct
  identifier(s)
  identifier(overlap_with_pointer)
  (
  struct
  identifier(s)
  *
  identifier(ptr)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  identifier(globvar)
  =
  identifier(overlap_with_globvar)
  (
  )
  ;
  if
  (
  identifier(globvar)
  .
  identifier(l1)
  !=
  const long(400l)
  ||
  identifier(globvar)
  .
  identifier(l2)
  !=
  const long(500l)
  ||
  identifier(globvar)
  .
  identifier(l3)
  !=
  const long(600l)
  )
  {
  return
  const int(2)
  ;
  }
  struct
  identifier(s)
  identifier(my_struct)
  =
  {
  const long(10l)
  ,
  const long(9l)
  ,
  const long(8l)
  }
  ;
  identifier(my_struct)
  =
  identifier(overlap_with_pointer)
  (
  &
  identifier(my_struct)
  )
  ;
  if
  (
  identifier(my_struct)
  .
  identifier(l1)
  !=
  const long(20l)
  ||
  identifier(my_struct)
  .
  identifier(l2)
  !=
  const long(18l)
  ||
  identifier(my_struct)
  .
  identifier(l3)
  !=
  const long(16l)
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
