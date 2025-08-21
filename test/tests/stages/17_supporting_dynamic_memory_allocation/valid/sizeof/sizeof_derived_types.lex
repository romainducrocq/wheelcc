-- Lexing ... OK
+
+
@@ Tokens @@
List[166]:
  void
  *
  identifier(malloc)
  (
  unsigned
  long
  identifier(size)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  sizeof
  (
  int
  [
  const int(2)
  ]
  )
  !=
  const int(8)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  sizeof
  (
  char
  [
  const int(3)
  ]
  [
  const int(6)
  ]
  [
  const int(17)
  ]
  [
  const int(9)
  ]
  )
  !=
  const int(2754)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  sizeof
  (
  int
  [
  const long(4294967297L)
  ]
  [
  const int(100000000)
  ]
  )
  !=
  const long(1717986918800000000l)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  sizeof
  (
  int
  *
  )
  !=
  const int(8)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  sizeof
  (
  int
  (
  *
  )
  [
  const int(2)
  ]
  [
  const int(4)
  ]
  [
  const int(6)
  ]
  )
  !=
  const int(8)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  sizeof
  (
  char
  *
  )
  !=
  const int(8)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  sizeof
  (
  double
  (
  *
  (
  [
  const int(3)
  ]
  [
  const int(4)
  ]
  )
  )
  [
  const int(2)
  ]
  )
  !=
  const int(96)
  )
  {
  return
  const int(7)
  ;
  }
  return
  const int(0)
  ;
  }
