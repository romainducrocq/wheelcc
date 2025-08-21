-- Lexing ... OK
+
+
@@ Tokens @@
List[197]:
  struct
  identifier(s)
  {
  int
  identifier(i)
  ;
  char
  identifier(arr)
  [
  const int(2)
  ]
  ;
  double
  identifier(d)
  ;
  }
  ;
  struct
  identifier(outer)
  {
  char
  identifier(c)
  ;
  struct
  identifier(s)
  identifier(inner)
  ;
  }
  ;
  extern
  struct
  identifier(s)
  identifier(global)
  ;
  extern
  struct
  identifier(outer)
  identifier(global_outer)
  ;
  void
  identifier(update_struct)
  (
  void
  )
  ;
  void
  identifier(update_outer_struct)
  (
  void
  )
  ;
  struct
  identifier(s)
  identifier(global)
  =
  {
  const int(1)
  ,
  {
  const int(2)
  ,
  const int(3)
  }
  ,
  const double(4.0)
  }
  ;
  struct
  identifier(outer)
  identifier(global_outer)
  =
  {
  const int(5)
  ,
  {
  const int(6)
  ,
  {
  const int(7)
  ,
  const int(8)
  }
  ,
  const double(9.0)
  }
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  identifier(update_struct)
  (
  )
  ;
  if
  (
  identifier(global)
  .
  identifier(arr)
  [
  const int(1)
  ]
  !=
  const int(4)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(global)
  .
  identifier(d)
  !=
  const double(5.0)
  )
  {
  return
  const int(2)
  ;
  }
  identifier(update_outer_struct)
  (
  )
  ;
  if
  (
  identifier(global_outer)
  .
  identifier(c)
  !=
  const int(5)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(global_outer)
  .
  identifier(inner)
  .
  identifier(i)
  ||
  identifier(global_outer)
  .
  identifier(inner)
  .
  identifier(d)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(global_outer)
  .
  identifier(inner)
  .
  identifier(arr)
  [
  const int(0)
  ]
  !=
  -
  const int(1)
  ||
  identifier(global_outer)
  .
  identifier(inner)
  .
  identifier(arr)
  [
  const int(1)
  ]
  !=
  -
  const int(1)
  )
  {
  return
  const int(5)
  ;
  }
  return
  const int(0)
  ;
  }
