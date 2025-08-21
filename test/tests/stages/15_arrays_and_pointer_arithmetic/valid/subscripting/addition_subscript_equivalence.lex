-- Lexing ... OK
+
+
@@ Tokens @@
List[226]:
  int
  identifier(main)
  (
  void
  )
  {
  unsigned
  long
  identifier(x)
  [
  const int(300)
  ]
  [
  const int(5)
  ]
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
  const int(300)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  for
  (
  int
  identifier(j)
  =
  const int(0)
  ;
  identifier(j)
  <
  const int(5)
  ;
  identifier(j)
  =
  identifier(j)
  +
  const int(1)
  )
  {
  identifier(x)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  =
  identifier(i)
  *
  const int(5)
  +
  identifier(j)
  ;
  }
  }
  if
  (
  *
  (
  *
  (
  identifier(x)
  +
  const int(20)
  )
  +
  const int(3)
  )
  !=
  identifier(x)
  [
  const int(20)
  ]
  [
  const int(3)
  ]
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  &
  (
  *
  (
  *
  (
  identifier(x)
  +
  const int(290)
  )
  +
  const int(3)
  )
  )
  !=
  &
  identifier(x)
  [
  const int(290)
  ]
  [
  const int(3)
  ]
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
  const int(300)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  for
  (
  int
  identifier(j)
  =
  const int(0)
  ;
  identifier(j)
  <
  const int(5)
  ;
  identifier(j)
  =
  identifier(j)
  +
  const int(1)
  )
  {
  if
  (
  *
  (
  *
  (
  identifier(x)
  +
  identifier(i)
  )
  +
  identifier(j)
  )
  !=
  identifier(x)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  )
  {
  return
  const int(3)
  ;
  }
  }
  }
  *
  (
  *
  (
  identifier(x)
  +
  const int(275)
  )
  +
  const int(4)
  )
  =
  const unsigned long(22000ul)
  ;
  if
  (
  identifier(x)
  [
  const int(275)
  ]
  [
  const int(4)
  ]
  !=
  const unsigned long(22000ul)
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
