-- Lexing ... OK
+
+
@@ Tokens @@
List[155]:
  int
  identifier(main)
  (
  void
  )
  {
  unsigned
  char
  identifier(chars)
  [
  const int(5)
  ]
  =
  {
  const int(0)
  ,
  const int(2)
  ,
  const int(4)
  ,
  const int(253)
  ,
  const int(255)
  }
  ;
  if
  (
  identifier(chars)
  [
  const int(0)
  ]
  --
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(chars)
  [
  const int(1)
  ]
  ++
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
  --
  identifier(chars)
  [
  const int(3)
  ]
  !=
  const int(252)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  ++
  identifier(chars)
  [
  const int(4)
  ]
  !=
  const int(0)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(chars)
  [
  const int(0)
  ]
  !=
  const int(255)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(chars)
  [
  const int(1)
  ]
  !=
  const int(3)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(chars)
  [
  const int(2)
  ]
  !=
  const int(4)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(chars)
  [
  const int(3)
  ]
  !=
  const int(252)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(chars)
  [
  const int(4)
  ]
  )
  {
  return
  const int(9)
  ;
  }
  return
  const int(0)
  ;
  }
