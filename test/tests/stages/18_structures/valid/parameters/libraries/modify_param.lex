-- Lexing ... OK
+
+
@@ Tokens @@
List[242]:
  struct
  identifier(inner)
  {
  double
  identifier(d)
  ;
  int
  identifier(i)
  ;
  }
  ;
  struct
  identifier(outer)
  {
  struct
  identifier(inner)
  identifier(s)
  ;
  struct
  identifier(inner)
  *
  identifier(ptr)
  ;
  long
  identifier(l)
  ;
  }
  ;
  int
  identifier(modify_simple_struct)
  (
  struct
  identifier(inner)
  identifier(s)
  )
  ;
  int
  identifier(modify_nested_struct)
  (
  struct
  identifier(outer)
  identifier(s)
  )
  ;
  int
  identifier(modify_simple_struct)
  (
  struct
  identifier(inner)
  identifier(s)
  )
  {
  struct
  identifier(inner)
  identifier(copy)
  =
  identifier(s)
  ;
  identifier(s)
  .
  identifier(d)
  =
  const double(0.0)
  ;
  if
  (
  identifier(s)
  .
  identifier(d)
  ||
  identifier(s)
  .
  identifier(i)
  !=
  const int(3)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(copy)
  .
  identifier(d)
  !=
  const double(2.0)
  ||
  identifier(copy)
  .
  identifier(i)
  !=
  const int(3)
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
  identifier(modify_nested_struct)
  (
  struct
  identifier(outer)
  identifier(s)
  )
  {
  struct
  identifier(outer)
  identifier(copy)
  =
  identifier(s)
  ;
  identifier(s)
  .
  identifier(l)
  =
  const int(10)
  ;
  identifier(s)
  .
  identifier(s)
  .
  identifier(i)
  =
  const int(200)
  ;
  identifier(s)
  .
  identifier(ptr)
  ->
  identifier(d)
  =
  const double(10.0)
  ;
  identifier(s)
  .
  identifier(ptr)
  ->
  identifier(i)
  =
  const int(11)
  ;
  if
  (
  identifier(s)
  .
  identifier(s)
  .
  identifier(i)
  !=
  const int(200)
  ||
  identifier(s)
  .
  identifier(s)
  .
  identifier(d)
  !=
  const double(4.0)
  ||
  identifier(s)
  .
  identifier(l)
  !=
  const int(10)
  ||
  identifier(s)
  .
  identifier(ptr)
  ->
  identifier(d)
  !=
  const double(10.0)
  ||
  identifier(s)
  .
  identifier(ptr)
  ->
  identifier(i)
  !=
  const int(11)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(copy)
  .
  identifier(s)
  .
  identifier(i)
  !=
  const int(5)
  ||
  identifier(copy)
  .
  identifier(s)
  .
  identifier(d)
  !=
  const double(4.0)
  ||
  identifier(copy)
  .
  identifier(l)
  !=
  const int(1000)
  ||
  identifier(copy)
  .
  identifier(ptr)
  ->
  identifier(d)
  !=
  const double(10.0)
  ||
  identifier(copy)
  .
  identifier(ptr)
  ->
  identifier(i)
  !=
  const int(11)
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
