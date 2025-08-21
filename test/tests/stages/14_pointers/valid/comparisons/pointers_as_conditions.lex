-- Lexing ... OK
+
+
@@ Tokens @@
List[176]:
  long
  *
  identifier(get_null_pointer)
  (
  void
  )
  {
  return
  const int(0)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  long
  identifier(x)
  ;
  long
  *
  identifier(ptr)
  =
  &
  identifier(x)
  ;
  long
  *
  identifier(null_ptr)
  =
  identifier(get_null_pointer)
  (
  )
  ;
  if
  (
  const double(5.0)
  &&
  identifier(null_ptr)
  )
  {
  return
  const int(1)
  ;
  }
  int
  identifier(a)
  =
  const int(0)
  ;
  if
  (
  !
  (
  identifier(ptr)
  ||
  (
  identifier(a)
  =
  const int(10)
  )
  )
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(a)
  !=
  const int(0)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  !
  identifier(ptr)
  )
  {
  return
  const int(4)
  ;
  }
  int
  identifier(j)
  =
  identifier(ptr)
  ?
  const int(1)
  :
  const int(2)
  ;
  int
  identifier(k)
  =
  identifier(null_ptr)
  ?
  const int(3)
  :
  const int(4)
  ;
  if
  (
  identifier(j)
  !=
  const int(1)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(k)
  !=
  const int(4)
  )
  {
  return
  const int(6)
  ;
  }
  int
  identifier(i)
  =
  const int(0)
  ;
  while
  (
  identifier(ptr)
  )
  {
  if
  (
  identifier(i)
  >=
  const int(10)
  )
  {
  identifier(ptr)
  =
  const int(0)
  ;
  continue
  ;
  }
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  ;
  }
  if
  (
  identifier(i)
  !=
  const int(10)
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
