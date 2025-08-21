-- Lexing ... OK
+
+
@@ Tokens @@
List[174]:
  struct
  identifier(inner)
  {
  long
  identifier(l)
  ;
  char
  identifier(arr)
  [
  const int(2)
  ]
  ;
  }
  ;
  struct
  identifier(outer)
  {
  char
  identifier(a)
  ;
  struct
  identifier(inner)
  identifier(b)
  ;
  }
  ;
  int
  identifier(validate_struct_array)
  (
  struct
  identifier(outer)
  *
  identifier(struct_array)
  )
  ;
  static
  struct
  identifier(outer)
  identifier(static_array)
  [
  const int(3)
  ]
  =
  {
  {
  const int(0)
  ,
  {
  const int(0)
  ,
  {
  const int(0)
  ,
  const int(0)
  }
  }
  }
  ,
  {
  const int(2)
  ,
  {
  const int(3)
  ,
  {
  const int(4)
  ,
  const int(5)
  }
  }
  }
  ,
  {
  const int(4)
  ,
  {
  const int(6)
  ,
  {
  const int(8)
  ,
  const int(10)
  }
  }
  }
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(outer)
  identifier(auto_array)
  [
  const int(3)
  ]
  =
  {
  {
  const int(0)
  ,
  {
  const int(0)
  ,
  {
  const int(0)
  ,
  const int(0)
  }
  }
  }
  ,
  {
  const int(2)
  ,
  {
  const int(3)
  ,
  {
  const int(4)
  ,
  const int(5)
  }
  }
  }
  ,
  {
  const int(4)
  ,
  {
  const int(6)
  ,
  {
  const int(8)
  ,
  const int(10)
  }
  }
  }
  }
  ;
  if
  (
  !
  identifier(validate_struct_array)
  (
  identifier(static_array)
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
  identifier(validate_struct_array)
  (
  identifier(auto_array)
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
