-- Lexing ... OK
+
+
@@ Tokens @@
List[276]:
  int
  identifier(check_alignment)
  (
  int
  *
  identifier(ptr)
  )
  {
  unsigned
  long
  identifier(addr)
  =
  (
  unsigned
  long
  )
  identifier(ptr)
  ;
  return
  (
  identifier(addr)
  %
  const int(16)
  ==
  const int(0)
  )
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(arr)
  [
  const int(5)
  ]
  =
  {
  const int(0)
  }
  ;
  int
  identifier(arr2)
  [
  const int(7)
  ]
  =
  {
  const int(0)
  }
  ;
  int
  identifier(arr3)
  [
  const int(2)
  ]
  [
  const int(2)
  ]
  =
  {
  {
  const int(0)
  }
  }
  ;
  if
  (
  !
  identifier(check_alignment)
  (
  identifier(arr)
  )
  )
  {
  return
  const int(1)
  ;
  }
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(5)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  identifier(arr)
  [
  identifier(i)
  ]
  =
  identifier(i)
  ;
  if
  (
  !
  identifier(check_alignment)
  (
  identifier(arr2)
  )
  )
  {
  return
  const int(2)
  ;
  }
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(7)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  if
  (
  identifier(arr2)
  [
  identifier(i)
  ]
  )
  return
  const int(3)
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(7)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  identifier(arr2)
  [
  identifier(i)
  ]
  =
  -
  identifier(i)
  ;
  }
  if
  (
  !
  identifier(check_alignment)
  (
  (
  int
  *
  )
  identifier(arr3)
  )
  )
  {
  return
  const int(4)
  ;
  }
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(5)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  if
  (
  identifier(arr)
  [
  identifier(i)
  ]
  !=
  identifier(i)
  )
  {
  return
  const int(5)
  ;
  }
  }
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(2)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  for
  (
  int
  identifier(j)
  =
  const int(0)
  ;
  identifier(j)
  <
  const int(2)
  ;
  identifier(j)
  =
  identifier(j)
  +
  const int(1)
  )
  if
  (
  identifier(arr3)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  !=
  const int(0)
  )
  return
  const int(6)
  ;
  return
  const int(0)
  ;
  }
