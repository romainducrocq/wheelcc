-- Lexing ... OK
+
+
@@ Tokens @@
List[197]:
  int
  identifier(main)
  (
  void
  )
  {
  static
  char
  identifier(chars)
  [
  const int(5)
  ]
  =
  {
  const int(123)
  ,
  const int(124)
  ,
  const int(125)
  ,
  const int(126)
  ,
  const int(127)
  }
  ;
  if
  (
  identifier(chars)
  [
  const int(0)
  ]
  ++
  !=
  const int(123)
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
  --
  !=
  const int(124)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  ++
  identifier(chars)
  [
  const int(2)
  ]
  !=
  const int(126)
  )
  {
  return
  const int(3)
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
  const int(125)
  )
  {
  return
  const int(4)
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
  -
  const int(128)
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
  const int(0)
  ]
  !=
  const int(124)
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
  const int(1)
  ]
  !=
  const int(123)
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
  const int(2)
  ]
  !=
  const int(126)
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
  const int(3)
  ]
  !=
  const int(125)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(chars)
  [
  const int(4)
  ]
  !=
  -
  const int(128)
  )
  {
  return
  const int(10)
  ;
  }
  signed
  char
  identifier(c)
  =
  -
  const int(128)
  ;
  identifier(c)
  --
  ;
  if
  (
  identifier(c)
  !=
  const int(127)
  )
  {
  return
  const int(11)
  ;
  }
  return
  const int(0)
  ;
  }
