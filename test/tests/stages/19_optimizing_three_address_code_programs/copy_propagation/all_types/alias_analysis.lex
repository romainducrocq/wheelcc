-- Lexing ... OK
+
+
@@ Tokens @@
List[138]:
  int
  identifier(callee)
  (
  int
  *
  identifier(ptr)
  )
  {
  if
  (
  *
  identifier(ptr)
  !=
  const int(10)
  )
  {
  return
  const int(0)
  ;
  }
  *
  identifier(ptr)
  =
  -
  const int(1)
  ;
  return
  const int(1)
  ;
  }
  int
  identifier(target)
  (
  int
  *
  identifier(ptr1)
  ,
  int
  *
  identifier(ptr2)
  )
  {
  int
  identifier(i)
  =
  const int(10)
  ;
  int
  identifier(j)
  =
  const int(20)
  ;
  *
  identifier(ptr1)
  =
  identifier(callee)
  (
  &
  identifier(i)
  )
  ;
  *
  identifier(ptr2)
  =
  identifier(i)
  ;
  identifier(i)
  =
  const int(4)
  ;
  return
  identifier(i)
  +
  identifier(j)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(callee_check1)
  ;
  int
  identifier(callee_check2)
  ;
  int
  identifier(result)
  =
  identifier(target)
  (
  &
  identifier(callee_check1)
  ,
  &
  identifier(callee_check2)
  )
  ;
  if
  (
  identifier(callee_check1)
  !=
  const int(1)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(callee_check2)
  !=
  -
  const int(1)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(result)
  !=
  const int(24)
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
