-- Lexing ... OK
+
+
@@ Tokens @@
List[178]:
  double
  *
  identifier(d)
  =
  const long(0l)
  ;
  int
  *
  identifier(i)
  =
  const unsigned long(0ul)
  ;
  int
  *
  identifier(i2)
  =
  const unsigned int(0u)
  ;
  int
  identifier(expect_null_param)
  (
  int
  *
  identifier(val)
  )
  {
  return
  (
  identifier(val)
  ==
  const unsigned long(0ul)
  )
  ;
  }
  long
  *
  identifier(return_null_ptr)
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
  int
  identifier(x)
  =
  const int(10)
  ;
  int
  *
  identifier(ptr)
  =
  &
  identifier(x)
  ;
  if
  (
  identifier(d)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(i)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(i2)
  )
  {
  return
  const int(3)
  ;
  }
  identifier(ptr)
  =
  const unsigned long(0ul)
  ;
  if
  (
  identifier(ptr)
  )
  {
  return
  const int(4)
  ;
  }
  int
  *
  identifier(y)
  =
  const int(0)
  ;
  if
  (
  identifier(y)
  !=
  const int(0)
  )
  return
  const int(5)
  ;
  if
  (
  !
  identifier(expect_null_param)
  (
  const int(0)
  )
  )
  {
  return
  const int(6)
  ;
  }
  long
  *
  identifier(null_ptr)
  =
  identifier(return_null_ptr)
  (
  )
  ;
  if
  (
  identifier(null_ptr)
  !=
  const int(0)
  )
  {
  return
  const int(7)
  ;
  }
  identifier(ptr)
  =
  &
  identifier(x)
  ;
  int
  *
  identifier(ternary_result)
  =
  const int(10)
  ?
  const int(0)
  :
  identifier(ptr)
  ;
  if
  (
  identifier(ternary_result)
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
