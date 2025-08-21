-- Lexing ... OK
+
+
@@ Tokens @@
List[241]:
  int
  identifier(test_shared_member_names)
  (
  void
  )
  {
  union
  identifier(u1)
  {
  int
  identifier(a)
  ;
  }
  ;
  union
  identifier(u2)
  {
  long
  identifier(l)
  ;
  double
  identifier(a)
  ;
  }
  ;
  struct
  identifier(s)
  {
  char
  identifier(a)
  [
  const int(2)
  ]
  ;
  }
  ;
  union
  identifier(u1)
  identifier(var1)
  =
  {
  const int(10)
  }
  ;
  union
  identifier(u2)
  identifier(var2)
  =
  {
  -
  const long(9223372036854775807l)
  -
  const int(1)
  }
  ;
  struct
  identifier(s)
  identifier(var3)
  =
  {
  {
  -
  const int(1)
  ,
  -
  const int(2)
  }
  }
  ;
  if
  (
  identifier(var1)
  .
  identifier(a)
  !=
  const int(10)
  ||
  identifier(var2)
  .
  identifier(a)
  !=
  -
  const double(0.0)
  ||
  identifier(var3)
  .
  identifier(a)
  [
  const int(0)
  ]
  !=
  -
  const int(1)
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
  identifier(test_same_name_var_member_and_tag)
  (
  void
  )
  {
  union
  identifier(u)
  {
  int
  identifier(u)
  ;
  }
  ;
  union
  identifier(u)
  identifier(u)
  =
  {
  const int(100)
  }
  ;
  if
  (
  identifier(u)
  .
  identifier(u)
  !=
  const int(100)
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
  identifier(f)
  (
  void
  )
  {
  return
  const int(10)
  ;
  }
  union
  identifier(f)
  {
  int
  identifier(f)
  ;
  }
  ;
  int
  identifier(test_same_name_fun_and_tag)
  (
  void
  )
  {
  union
  identifier(f)
  identifier(x)
  ;
  identifier(x)
  .
  identifier(f)
  =
  identifier(f)
  (
  )
  ;
  if
  (
  identifier(x)
  .
  identifier(f)
  !=
  const int(10)
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
  identifier(test_shared_member_names)
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
  identifier(test_same_name_var_member_and_tag)
  (
  )
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  !
  identifier(test_same_name_fun_and_tag)
  (
  )
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
