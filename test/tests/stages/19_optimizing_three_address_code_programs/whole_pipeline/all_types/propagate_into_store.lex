-- Lexing ... OK
+
+
@@ Tokens @@
List[86]:
  struct
  identifier(s)
  {
  int
  identifier(a)
  ;
  int
  identifier(b)
  ;
  }
  ;
  int
  identifier(glob)
  =
  const int(0)
  ;
  int
  identifier(i)
  =
  const int(0)
  ;
  int
  identifier(target)
  (
  void
  )
  {
  int
  *
  identifier(ptr)
  =
  &
  identifier(i)
  ;
  identifier(glob)
  =
  const int(30)
  ;
  *
  identifier(ptr)
  =
  identifier(glob)
  ;
  identifier(glob)
  =
  const int(10)
  ;
  return
  *
  identifier(ptr)
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
  )
  !=
  const int(30)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(glob)
  !=
  const int(10)
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
