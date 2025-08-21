-- Lexing ... OK
+
+
@@ Tokens @@
List[212]:
  unsigned
  int
  identifier(w)
  =
  const unsigned int(4294967295U)
  ;
  int
  identifier(x)
  =
  const int(10)
  ;
  unsigned
  int
  identifier(y)
  =
  const unsigned int(4294967295U)
  ;
  double
  *
  identifier(dbl_ptr)
  ;
  long
  identifier(modify_ptr)
  (
  long
  *
  identifier(new_ptr)
  )
  {
  static
  long
  *
  identifier(p)
  ;
  if
  (
  identifier(new_ptr)
  )
  {
  identifier(p)
  =
  identifier(new_ptr)
  ;
  }
  return
  *
  identifier(p)
  ;
  }
  int
  identifier(increment_ptr)
  (
  void
  )
  {
  *
  identifier(dbl_ptr)
  =
  *
  identifier(dbl_ptr)
  +
  const double(5.0)
  ;
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
  int
  *
  identifier(pointer_to_static)
  =
  &
  identifier(x)
  ;
  identifier(x)
  =
  const int(20)
  ;
  if
  (
  *
  identifier(pointer_to_static)
  !=
  const int(20)
  )
  {
  return
  const int(1)
  ;
  }
  *
  identifier(pointer_to_static)
  =
  const int(100)
  ;
  if
  (
  identifier(x)
  !=
  const int(100)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(w)
  !=
  const unsigned int(4294967295U)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(y)
  !=
  const unsigned int(4294967295U)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(dbl_ptr)
  )
  {
  return
  const int(5)
  ;
  }
  long
  identifier(l)
  =
  const long(1000l)
  ;
  if
  (
  identifier(modify_ptr)
  (
  &
  identifier(l)
  )
  !=
  const long(1000l)
  )
  {
  return
  const int(6)
  ;
  }
  identifier(l)
  =
  -
  const int(1)
  ;
  if
  (
  identifier(modify_ptr)
  (
  const int(0)
  )
  !=
  identifier(l)
  )
  {
  return
  const int(7)
  ;
  }
  double
  identifier(d)
  =
  const double(10.0)
  ;
  identifier(dbl_ptr)
  =
  &
  identifier(d)
  ;
  identifier(increment_ptr)
  (
  )
  ;
  if
  (
  *
  identifier(dbl_ptr)
  !=
  const int(15)
  )
  {
  return
  const int(8)
  ;
  }
  return
  const int(0)
  ;
  }
