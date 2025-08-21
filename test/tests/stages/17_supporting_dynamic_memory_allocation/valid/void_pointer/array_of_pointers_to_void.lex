-- Lexing ... OK
+
+
@@ Tokens @@
List[131]:
  void
  *
  identifier(calloc)
  (
  unsigned
  long
  identifier(nmemb)
  ,
  unsigned
  long
  identifier(size)
  )
  ;
  void
  identifier(free)
  (
  void
  *
  identifier(ptr)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(i)
  =
  const int(10)
  ;
  void
  *
  identifier(arr)
  [
  const int(4)
  ]
  =
  {
  identifier(calloc)
  (
  const int(2)
  ,
  sizeof
  (
  int
  )
  )
  ,
  &
  identifier(i)
  ,
  const int(0)
  ,
  identifier(arr)
  }
  ;
  long
  *
  identifier(l)
  =
  identifier(arr)
  [
  const int(0)
  ]
  ;
  if
  (
  *
  identifier(l)
  )
  return
  const int(1)
  ;
  int
  identifier(elem_1_val)
  =
  *
  (
  int
  *
  )
  identifier(arr)
  [
  const int(1)
  ]
  ;
  if
  (
  identifier(elem_1_val)
  !=
  const int(10)
  )
  return
  const int(2)
  ;
  if
  (
  identifier(arr)
  [
  const int(2)
  ]
  )
  return
  const int(3)
  ;
  if
  (
  identifier(arr)
  [
  const int(3)
  ]
  !=
  identifier(arr)
  )
  return
  const int(4)
  ;
  identifier(free)
  (
  identifier(arr)
  [
  const int(0)
  ]
  )
  ;
  return
  const int(0)
  ;
  }
