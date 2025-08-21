-- Lexing ... OK
+
+
@@ Tokens @@
List[126]:
  int
  identifier(callee)
  (
  int
  identifier(a)
  ,
  int
  identifier(b)
  )
  {
  return
  identifier(a)
  *
  identifier(b)
  ;
  }
  int
  identifier(f)
  (
  void
  )
  {
  return
  const int(3)
  ;
  }
  int
  identifier(globl)
  =
  const int(0)
  ;
  int
  identifier(set_globvar)
  (
  void
  )
  {
  identifier(globl)
  =
  const int(4)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(target)
  (
  int
  identifier(a)
  ,
  int
  identifier(b)
  )
  {
  identifier(b)
  =
  identifier(a)
  ;
  identifier(set_globvar)
  (
  )
  ;
  int
  identifier(product)
  =
  identifier(callee)
  (
  identifier(a)
  ,
  identifier(b)
  )
  ;
  identifier(b)
  =
  identifier(f)
  (
  )
  ;
  return
  (
  identifier(product)
  +
  identifier(a)
  -
  identifier(b)
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
  identifier(target)
  (
  const int(5)
  ,
  const int(6)
  )
  !=
  const int(27)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(globl)
  !=
  const int(4)
  )
  {
  return
  const int(2)
  ;
  }
  return
  const int(0)
  ;
  }
