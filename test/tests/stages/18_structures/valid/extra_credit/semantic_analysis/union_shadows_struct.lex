-- Lexing ... OK
+
+
@@ Tokens @@
List[76]:
  struct
  identifier(tag)
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
  identifier(tag)
  identifier(global_struct)
  =
  {
  const int(1)
  ,
  const int(2)
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  union
  identifier(tag)
  {
  int
  identifier(x)
  ;
  long
  identifier(y)
  ;
  }
  ;
  union
  identifier(tag)
  identifier(local_union)
  =
  {
  const int(100)
  }
  ;
  if
  (
  identifier(global_struct)
  .
  identifier(a)
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
  identifier(local_union)
  .
  identifier(x)
  !=
  const int(100)
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
