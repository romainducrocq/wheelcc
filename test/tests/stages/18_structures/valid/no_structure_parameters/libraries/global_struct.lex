-- Lexing ... OK
+
+
@@ Tokens @@
List[111]:
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
  void
  identifier(update_struct)
  (
  void
  )
  {
  identifier(global)
  .
  identifier(arr)
  [
  const int(1)
  ]
  =
  identifier(global)
  .
  identifier(arr)
  [
  const int(0)
  ]
  *
  const int(2)
  ;
  identifier(global)
  .
  identifier(d)
  =
  const double(5.0)
  ;
  }
  void
  identifier(update_outer_struct)
  (
  void
  )
  {
  struct
  identifier(s)
  identifier(inner)
  =
  {
  const int(0)
  ,
  {
  -
  const int(1)
  ,
  -
  const int(1)
  }
  ,
  const int(0)
  }
  ;
  identifier(global_outer)
  .
  identifier(inner)
  =
  identifier(inner)
  ;
  }
