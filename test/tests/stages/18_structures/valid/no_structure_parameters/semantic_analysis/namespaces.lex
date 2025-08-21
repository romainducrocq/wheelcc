-- Lexing ... OK
+
+
@@ Tokens @@
List[320]:
  int
  identifier(test_shared_member_names)
  (
  void
  )
  {
  struct
  identifier(pair1)
  {
  int
  identifier(x)
  ;
  int
  identifier(y)
  ;
  }
  ;
  struct
  identifier(pair2)
  {
  double
  identifier(x)
  ;
  char
  identifier(y)
  ;
  }
  ;
  struct
  identifier(pair1)
  identifier(p1)
  =
  {
  const int(1)
  ,
  const int(2)
  }
  ;
  struct
  identifier(pair2)
  identifier(p2)
  =
  {
  const double(3.0)
  ,
  const int(4)
  }
  ;
  if
  (
  identifier(p1)
  .
  identifier(x)
  !=
  const int(1)
  ||
  identifier(p2)
  .
  identifier(x)
  !=
  const double(3.0)
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
  identifier(test_shared_nested_member_names)
  (
  void
  )
  {
  struct
  identifier(pair1)
  {
  int
  identifier(x)
  ;
  int
  *
  identifier(y)
  ;
  }
  ;
  struct
  identifier(pair2)
  {
  void
  *
  identifier(x)
  ;
  double
  identifier(y)
  [
  const int(4)
  ]
  ;
  }
  ;
  struct
  identifier(pair1)
  identifier(p1)
  =
  {
  const int(3)
  ,
  &
  (
  identifier(p1)
  .
  identifier(x)
  )
  }
  ;
  struct
  identifier(pair2)
  identifier(p2)
  =
  {
  &
  identifier(p1)
  ,
  {
  const double(1.0)
  ,
  const double(2.0)
  ,
  const double(3.0)
  ,
  const double(4.0)
  }
  }
  ;
  if
  (
  (
  (
  struct
  identifier(pair1)
  *
  )
  identifier(p2)
  .
  identifier(x)
  )
  ->
  identifier(x)
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
  identifier(test_same_name_var_member_and_tag)
  (
  void
  )
  {
  struct
  identifier(x)
  {
  int
  identifier(x)
  ;
  }
  ;
  struct
  identifier(x)
  identifier(x)
  =
  {
  const int(10)
  }
  ;
  if
  (
  identifier(x)
  .
  identifier(x)
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
  identifier(test_same_name_fun_member_and_tag)
  (
  void
  )
  {
  struct
  identifier(f)
  {
  int
  identifier(f)
  ;
  }
  ;
  int
  identifier(f)
  (
  void
  )
  ;
  struct
  identifier(f)
  identifier(my_struct)
  ;
  identifier(my_struct)
  .
  identifier(f)
  =
  identifier(f)
  (
  )
  ;
  if
  (
  identifier(my_struct)
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
  identifier(f)
  (
  void
  )
  {
  return
  const int(10)
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
  identifier(test_shared_nested_member_names)
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
  identifier(test_same_name_var_member_and_tag)
  (
  )
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  !
  identifier(test_same_name_fun_member_and_tag)
  (
  )
  )
  {
  return
  const int(4)
  ;
  }
  return
  const int(0)
  ;
  }
