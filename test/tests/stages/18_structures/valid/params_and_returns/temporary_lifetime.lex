-- Lexing ... OK
+
+
@@ Tokens @@
List[115]:
  struct
  identifier(s)
  {
  int
  identifier(arr)
  [
  const int(3)
  ]
  ;
  }
  ;
  struct
  identifier(s)
  identifier(f)
  (
  void
  )
  {
  struct
  identifier(s)
  identifier(retval)
  =
  {
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  }
  }
  ;
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
  int
  identifier(i)
  =
  identifier(f)
  (
  )
  .
  identifier(arr)
  [
  const int(0)
  ]
  ;
  int
  identifier(j)
  =
  identifier(f)
  (
  )
  .
  identifier(arr)
  [
  const int(1)
  ]
  ;
  int
  identifier(k)
  =
  identifier(f)
  (
  )
  .
  identifier(arr)
  [
  const int(2)
  ]
  ;
  if
  (
  identifier(i)
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
  identifier(j)
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
  identifier(k)
  !=
  const int(3)
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
