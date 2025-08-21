-- Lexing ... OK
+
+
@@ Tokens @@
List[95]:
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
  struct
  identifier(s)
  identifier(glob)
  ;
  int
  identifier(target)
  (
  void
  )
  {
  struct
  identifier(s)
  identifier(loc)
  =
  {
  const int(100)
  ,
  const int(200)
  }
  ;
  identifier(glob)
  =
  identifier(loc)
  ;
  int
  identifier(x)
  =
  identifier(glob)
  .
  identifier(b)
  ;
  return
  identifier(x)
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
  const int(200)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(glob)
  .
  identifier(a)
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
  identifier(glob)
  .
  identifier(b)
  !=
  const int(200)
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
