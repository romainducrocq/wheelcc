-- Lexing ... OK
+
+
@@ Tokens @@
List[357]:
  int
  identifier(test_jz)
  (
  int
  identifier(flag)
  ,
  int
  identifier(arg)
  )
  {
  if
  (
  identifier(flag)
  )
  {
  identifier(arg)
  =
  const int(0)
  ;
  }
  return
  identifier(arg)
  ?
  const int(1)
  :
  const int(2)
  ;
  }
  int
  identifier(test_jnz)
  (
  int
  identifier(flag)
  ,
  int
  identifier(arg)
  )
  {
  if
  (
  identifier(flag)
  )
  {
  identifier(arg)
  =
  const int(0)
  ;
  }
  return
  identifier(arg)
  ||
  const int(0)
  ;
  }
  int
  identifier(test_binary)
  (
  int
  identifier(flag)
  ,
  int
  identifier(arg1)
  ,
  int
  identifier(arg2)
  )
  {
  if
  (
  identifier(flag)
  ==
  const int(0)
  )
  {
  identifier(arg1)
  =
  const int(4)
  ;
  }
  else
  if
  (
  identifier(flag)
  ==
  const int(1)
  )
  {
  identifier(arg2)
  =
  const int(3)
  ;
  }
  return
  identifier(arg1)
  *
  identifier(arg2)
  ;
  }
  int
  identifier(test_unary)
  (
  int
  identifier(flag)
  ,
  int
  identifier(arg)
  )
  {
  if
  (
  identifier(flag)
  )
  {
  identifier(arg)
  =
  const int(5)
  ;
  }
  return
  -
  identifier(arg)
  ;
  }
  int
  identifier(f)
  (
  int
  identifier(arg)
  )
  {
  return
  identifier(arg)
  +
  const int(1)
  ;
  }
  int
  identifier(test_funcall)
  (
  int
  identifier(flag)
  ,
  int
  identifier(arg)
  )
  {
  if
  (
  identifier(flag)
  )
  {
  identifier(arg)
  =
  const int(7)
  ;
  }
  return
  identifier(f)
  (
  identifier(arg)
  )
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
  identifier(test_jz)
  (
  const int(1)
  ,
  const int(1)
  )
  !=
  const int(2)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(test_jz)
  (
  const int(0)
  ,
  const int(1)
  )
  !=
  const int(1)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(test_jnz)
  (
  const int(1)
  ,
  const int(1)
  )
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
  identifier(test_jnz)
  (
  const int(0)
  ,
  const int(1)
  )
  !=
  const int(1)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(test_binary)
  (
  const int(0)
  ,
  const int(8)
  ,
  const int(9)
  )
  !=
  const int(36)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(test_binary)
  (
  const int(1)
  ,
  const int(8)
  ,
  const int(9)
  )
  !=
  const int(24)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(test_binary)
  (
  const int(2)
  ,
  const int(8)
  ,
  const int(9)
  )
  !=
  const int(72)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(test_unary)
  (
  const int(0)
  ,
  const int(8)
  )
  !=
  -
  const int(8)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(test_unary)
  (
  const int(1)
  ,
  const int(8)
  )
  !=
  -
  const int(5)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(test_funcall)
  (
  const int(1)
  ,
  const int(5)
  )
  !=
  const int(8)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  identifier(test_funcall)
  (
  const int(0)
  ,
  const int(9)
  )
  !=
  const int(10)
  )
  {
  return
  const int(11)
  ;
  }
  return
  const int(0)
  ;
  }
