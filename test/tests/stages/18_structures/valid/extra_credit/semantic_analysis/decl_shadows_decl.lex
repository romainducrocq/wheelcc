-- Lexing ... OK
+
+
@@ Tokens @@
List[43]:
  int
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(tag)
  ;
  struct
  identifier(tag)
  *
  identifier(struct_ptr)
  =
  const int(0)
  ;
  {
  union
  identifier(tag)
  ;
  union
  identifier(tag)
  *
  identifier(union_ptr)
  =
  const int(0)
  ;
  if
  (
  identifier(struct_ptr)
  ||
  identifier(union_ptr)
  )
  {
  return
  const int(1)
  ;
  }
  }
  return
  const int(0)
  ;
  }
