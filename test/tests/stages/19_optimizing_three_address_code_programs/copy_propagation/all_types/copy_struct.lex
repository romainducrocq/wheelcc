-- Lexing ... OK
+
+
@@ Tokens @@
List[130]:
  struct
  identifier(s)
  {
  int
  identifier(x)
  ;
  int
  identifier(y)
  ;
  }
  ;
  int
  identifier(callee)
  (
  struct
  identifier(s)
  identifier(a)
  ,
  struct
  identifier(s)
  identifier(b)
  )
  {
  if
  (
  identifier(a)
  .
  identifier(x)
  !=
  const int(3)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(a)
  .
  identifier(y)
  !=
  const int(4)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(b)
  .
  identifier(x)
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
  identifier(b)
  .
  identifier(y)
  !=
  const int(4)
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
  int
  identifier(target)
  (
  void
  )
  {
  struct
  identifier(s)
  identifier(s1)
  =
  {
  const int(1)
  ,
  const int(2)
  }
  ;
  struct
  identifier(s)
  identifier(s2)
  =
  {
  const int(3)
  ,
  const int(4)
  }
  ;
  identifier(s1)
  =
  identifier(s2)
  ;
  return
  identifier(callee)
  (
  identifier(s1)
  ,
  identifier(s2)
  )
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  return
  identifier(target)
  (
  )
  ;
  }
