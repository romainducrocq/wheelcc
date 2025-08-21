-- Lexing ... OK
+
+
@@ Tokens @@
List[86]:
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
  int
  identifier(glob)
  =
  const int(0)
  ;
  int
  identifier(target)
  (
  void
  )
  {
  struct
  identifier(s)
  identifier(my_struct)
  =
  {
  const int(1)
  ,
  const int(2)
  }
  ;
  identifier(glob)
  =
  const int(30)
  ;
  identifier(my_struct)
  .
  identifier(b)
  =
  identifier(glob)
  ;
  identifier(glob)
  =
  const int(10)
  ;
  return
  identifier(my_struct)
  .
  identifier(b)
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
  identifier(target)
  (
  )
  !=
  const int(30)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(glob)
  !=
  const int(10)
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
