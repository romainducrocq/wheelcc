-- Lexing ... OK
+
+
@@ Tokens @@
List[374]:
  int
  identifier(target_if)
  (
  void
  )
  {
  if
  (
  const int(0)
  )
  return
  const int(1)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(target_if_else_true)
  (
  void
  )
  {
  if
  (
  const int(1)
  )
  {
  return
  const int(2)
  ;
  }
  else
  {
  return
  const int(3)
  ;
  }
  }
  int
  identifier(target_if_else_false)
  (
  void
  )
  {
  if
  (
  const int(0)
  )
  {
  return
  const int(2)
  ;
  }
  else
  {
  return
  const int(3)
  ;
  }
  }
  int
  identifier(target_conditional_true)
  (
  void
  )
  {
  return
  const int(1)
  ?
  const int(2)
  :
  const int(3)
  ;
  }
  int
  identifier(target_conditional_false)
  (
  void
  )
  {
  return
  const int(0)
  ?
  const int(4)
  :
  const int(5)
  ;
  }
  int
  identifier(target_do_loop)
  (
  void
  )
  {
  int
  identifier(retval)
  =
  const int(0)
  ;
  do
  {
  identifier(retval)
  =
  const int(10)
  ;
  }
  while
  (
  const int(0)
  )
  ;
  return
  identifier(retval)
  ;
  }
  int
  identifier(target_while_loop_false)
  (
  void
  )
  {
  int
  identifier(retval)
  =
  const int(0)
  ;
  while
  (
  const int(0)
  )
  {
  identifier(retval)
  =
  const int(10)
  ;
  }
  return
  identifier(retval)
  ;
  }
  int
  identifier(target_while_loop_true)
  (
  void
  )
  {
  int
  identifier(retval)
  =
  const int(0)
  ;
  while
  (
  const int(1048576)
  )
  {
  identifier(retval)
  =
  const int(10)
  ;
  break
  ;
  }
  return
  identifier(retval)
  ;
  }
  int
  identifier(target_for_loop_true)
  (
  void
  )
  {
  int
  identifier(retval)
  =
  const int(0)
  ;
  for
  (
  int
  identifier(i)
  =
  const int(100)
  ;
  const int(123)
  ;
  )
  {
  identifier(retval)
  =
  identifier(i)
  ;
  break
  ;
  }
  return
  identifier(retval)
  ;
  }
  int
  identifier(target_for_loop_false)
  (
  void
  )
  {
  int
  identifier(retval)
  =
  const int(0)
  ;
  for
  (
  int
  identifier(i)
  =
  const int(100)
  ;
  const int(0)
  ;
  )
  {
  identifier(retval)
  =
  identifier(i)
  ;
  break
  ;
  }
  return
  identifier(retval)
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
  identifier(target_if)
  (
  )
  !=
  const int(0)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(target_if_else_true)
  (
  )
  !=
  const int(2)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_if_else_false)
  (
  )
  !=
  const int(3)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_conditional_true)
  (
  )
  !=
  const int(2)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(target_conditional_false)
  (
  )
  !=
  const int(5)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(target_do_loop)
  (
  )
  !=
  const int(10)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(target_while_loop_false)
  (
  )
  !=
  const int(0)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(target_while_loop_true)
  (
  )
  !=
  const int(10)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(target_for_loop_true)
  (
  )
  !=
  const int(100)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(target_for_loop_false)
  (
  )
  !=
  const int(0)
  )
  {
  return
  const int(10)
  ;
  }
  return
  const int(0)
  ;
  }
