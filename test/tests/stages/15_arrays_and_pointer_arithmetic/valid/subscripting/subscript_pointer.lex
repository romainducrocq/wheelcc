-- Lexing ... OK
+
+
@@ Tokens @@
List[99]:
  int
  identifier(subscript_pointer_to_pointer)
  (
  int
  *
  *
  identifier(x)
  )
  {
  return
  identifier(x)
  [
  const int(0)
  ]
  [
  const int(0)
  ]
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(a)
  =
  const int(3)
  ;
  int
  *
  identifier(ptr)
  =
  &
  identifier(a)
  ;
  if
  (
  identifier(ptr)
  [
  const int(0)
  ]
  !=
  const int(3)
  )
  {
  return
  const int(1)
  ;
  }
  int
  *
  *
  identifier(ptr_ptr)
  =
  &
  identifier(ptr)
  ;
  if
  (
  identifier(ptr_ptr)
  [
  const int(0)
  ]
  [
  const int(0)
  ]
  !=
  const int(3)
  )
  {
  return
  const int(2)
  ;
  }
  int
  identifier(dereferenced)
  =
  identifier(subscript_pointer_to_pointer)
  (
  identifier(ptr_ptr)
  )
  ;
  if
  (
  identifier(dereferenced)
  !=
  const int(3)
  )
  {
  return
  const int(3)
  ;
  }
  return
  const int(0)
  ;
  }
