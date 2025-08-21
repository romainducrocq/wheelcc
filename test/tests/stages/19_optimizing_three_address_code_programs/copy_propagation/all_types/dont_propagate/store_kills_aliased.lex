-- Lexing ... OK
+
+
@@ Tokens @@
List[221]:
  int
  identifier(aliased_src)
  (
  int
  identifier(flag)
  ,
  int
  identifier(x)
  ,
  int
  *
  identifier(ptr)
  )
  {
  int
  identifier(y)
  =
  identifier(x)
  ;
  if
  (
  identifier(flag)
  )
  {
  identifier(ptr)
  =
  &
  identifier(x)
  ;
  }
  *
  identifier(ptr)
  =
  const int(100)
  ;
  return
  identifier(y)
  ;
  }
  int
  identifier(aliased_dst)
  (
  int
  identifier(flag)
  ,
  int
  identifier(x)
  ,
  int
  *
  identifier(ptr)
  )
  {
  int
  identifier(y)
  =
  identifier(x)
  ;
  if
  (
  identifier(flag)
  )
  {
  identifier(ptr)
  =
  &
  identifier(y)
  ;
  }
  *
  identifier(ptr)
  =
  const int(100)
  ;
  return
  identifier(y)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(i)
  =
  const int(0)
  ;
  if
  (
  identifier(aliased_src)
  (
  const int(0)
  ,
  const int(1)
  ,
  &
  identifier(i)
  )
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
  identifier(i)
  !=
  const int(100)
  )
  {
  return
  const int(2)
  ;
  }
  identifier(i)
  =
  const int(0)
  ;
  if
  (
  identifier(aliased_src)
  (
  const int(1)
  ,
  const int(2)
  ,
  &
  identifier(i)
  )
  !=
  const int(2)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(i)
  !=
  const int(0)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(aliased_dst)
  (
  const int(0)
  ,
  const int(5)
  ,
  &
  identifier(i)
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
  identifier(i)
  !=
  const int(100)
  )
  {
  return
  const int(6)
  ;
  }
  identifier(i)
  =
  const int(0)
  ;
  if
  (
  identifier(aliased_dst)
  (
  const int(1)
  ,
  const int(5)
  ,
  &
  identifier(i)
  )
  !=
  const int(100)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(i)
  !=
  const int(0)
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
