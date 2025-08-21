-- Lexing ... OK
+
+
@@ Tokens @@
List[87]:
  int
  identifier(addr_of_arg)
  (
  int
  identifier(a)
  )
  {
  int
  *
  identifier(ptr)
  =
  &
  identifier(a)
  ;
  *
  identifier(ptr)
  =
  const int(10)
  ;
  return
  identifier(a)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(result)
  =
  identifier(addr_of_arg)
  (
  -
  const int(20)
  )
  ;
  if
  (
  identifier(result)
  !=
  const int(10)
  )
  {
  return
  const int(1)
  ;
  }
  int
  identifier(var)
  =
  const int(100)
  ;
  identifier(result)
  =
  identifier(addr_of_arg)
  (
  identifier(var)
  )
  ;
  if
  (
  identifier(result)
  !=
  const int(10)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(var)
  !=
  const int(100)
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
