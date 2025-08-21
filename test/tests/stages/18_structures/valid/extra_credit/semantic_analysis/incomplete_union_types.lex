-- Lexing ... OK
+
+
@@ Tokens @@
List[283]:
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
  int
  identifier(puts)
  (
  char
  *
  identifier(s)
  )
  ;
  union
  identifier(never_used)
  ;
  union
  identifier(never_used)
  identifier(incomplete_fun)
  (
  union
  identifier(never_used)
  identifier(x)
  )
  ;
  int
  identifier(test_block_scope_forward_decl)
  (
  void
  )
  {
  union
  identifier(u)
  ;
  union
  identifier(u)
  *
  identifier(u_ptr)
  =
  const int(0)
  ;
  union
  identifier(u)
  {
  long
  identifier(x)
  ;
  char
  identifier(y)
  ;
  }
  ;
  union
  identifier(u)
  identifier(val)
  =
  {
  -
  const long(100000000l)
  }
  ;
  identifier(u_ptr)
  =
  &
  identifier(val)
  ;
  if
  (
  identifier(u_ptr)
  ->
  identifier(x)
  !=
  -
  const long(100000000l)
  ||
  identifier(u_ptr)
  ->
  identifier(y)
  !=
  const int(0)
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
  union
  identifier(opaque_union)
  ;
  union
  identifier(opaque_union)
  *
  identifier(use_union_pointers)
  (
  union
  identifier(opaque_union)
  *
  identifier(param)
  )
  {
  if
  (
  identifier(param)
  ==
  const int(0)
  )
  {
  identifier(puts)
  (
  string literal("null pointer")
  )
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(test_use_incomplete_union_pointers)
  (
  void
  )
  {
  union
  identifier(opaque_union)
  *
  identifier(ptr1)
  =
  identifier(calloc)
  (
  const int(1)
  ,
  const int(4)
  )
  ;
  union
  identifier(opaque_union)
  *
  identifier(ptr2)
  =
  identifier(calloc)
  (
  const int(1)
  ,
  const int(4)
  )
  ;
  char
  *
  identifier(ptr1_bytes)
  =
  (
  char
  *
  )
  identifier(ptr1)
  ;
  if
  (
  identifier(ptr1_bytes)
  [
  const int(0)
  ]
  ||
  identifier(ptr1_bytes)
  [
  const int(1)
  ]
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(ptr1)
  ==
  const int(0)
  ||
  identifier(ptr2)
  ==
  const int(0)
  ||
  identifier(ptr1)
  ==
  identifier(ptr2)
  )
  {
  return
  const int(0)
  ;
  }
  static
  int
  identifier(flse)
  =
  const int(0)
  ;
  union
  identifier(opaque_union)
  *
  identifier(ptr3)
  =
  identifier(flse)
  ?
  identifier(ptr1)
  :
  identifier(ptr2)
  ;
  if
  (
  identifier(ptr3)
  !=
  identifier(ptr2)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(use_union_pointers)
  (
  identifier(ptr3)
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
  identifier(main)
  (
  void
  )
  {
  if
  (
  !
  identifier(test_block_scope_forward_decl)
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
  identifier(test_use_incomplete_union_pointers)
  (
  )
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
