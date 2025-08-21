-- Lexing ... OK
+
+
@@ Tokens @@
List[179]:
  int
  *
  identifier(return_pointer)
  (
  void
  )
  {
  static
  int
  identifier(var)
  =
  const int(10)
  ;
  return
  &
  identifier(var)
  ;
  }
  int
  identifier(one)
  =
  const int(1)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(val)
  =
  const int(100)
  ;
  int
  *
  identifier(ptr_var)
  =
  &
  identifier(val)
  ;
  if
  (
  *
  identifier(return_pointer)
  (
  )
  !=
  const int(10)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  *
  (
  identifier(one)
  ?
  identifier(return_pointer)
  (
  )
  :
  identifier(ptr_var)
  )
  !=
  const int(10)
  )
  return
  const int(2)
  ;
  if
  (
  *
  (
  identifier(one)
  -
  const int(1)
  ?
  identifier(return_pointer)
  (
  )
  :
  identifier(ptr_var)
  )
  !=
  const int(100)
  )
  {
  return
  const int(3)
  ;
  }
  int
  *
  identifier(ptr_to_one)
  =
  &
  identifier(one)
  ;
  if
  (
  *
  (
  identifier(ptr_var)
  =
  identifier(ptr_to_one)
  )
  !=
  const int(1)
  )
  {
  return
  const int(4)
  ;
  }
  *
  identifier(return_pointer)
  (
  )
  =
  const int(20)
  ;
  *
  (
  identifier(one)
  ?
  identifier(ptr_var)
  :
  identifier(return_pointer)
  (
  )
  )
  =
  const int(30)
  ;
  if
  (
  *
  identifier(return_pointer)
  (
  )
  !=
  const int(20)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  *
  identifier(ptr_var)
  !=
  const int(30)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(one)
  !=
  const int(30)
  )
  {
  return
  const int(7)
  ;
  }
  return
  const int(0)
  ;
  }
