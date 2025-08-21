-- Lexing ... OK
+
+
@@ Tokens @@
List[87]:
  struct
  identifier(s)
  {
  int
  identifier(a)
  ;
  int
  identifier(b)
  ;
  int
  identifier(c)
  ;
  }
  ;
  struct
  identifier(s)
  identifier(glob)
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(s)
  identifier(my_struct)
  =
  identifier(glob)
  ;
  identifier(my_struct)
  .
  identifier(c)
  =
  const int(100)
  ;
  if
  (
  identifier(my_struct)
  .
  identifier(c)
  !=
  const int(100)
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
  identifier(a)
  !=
  const int(1)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(glob)
  .
  identifier(c)
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
