-- Lexing ... OK
+
+
@@ Tokens @@
List[249]:
  char
  identifier(static1)
  [
  const int(4)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  }
  ;
  signed
  char
  identifier(static2)
  [
  const int(4)
  ]
  =
  {
  const int(3)
  ,
  const int(4)
  }
  ;
  unsigned
  char
  identifier(static3)
  [
  const int(3)
  ]
  =
  {
  const int(5)
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(static1)
  [
  const int(0)
  ]
  !=
  const int(1)
  ||
  identifier(static1)
  [
  const int(1)
  ]
  !=
  const int(2)
  ||
  identifier(static1)
  [
  const int(2)
  ]
  ||
  identifier(static1)
  [
  const int(3)
  ]
  )
  return
  const int(1)
  ;
  if
  (
  identifier(static2)
  [
  const int(0)
  ]
  !=
  const int(3)
  ||
  identifier(static2)
  [
  const int(1)
  ]
  !=
  const int(4)
  ||
  identifier(static2)
  [
  const int(2)
  ]
  ||
  identifier(static2)
  [
  const int(3)
  ]
  )
  return
  const int(2)
  ;
  if
  (
  identifier(static3)
  [
  const int(0)
  ]
  !=
  const int(5)
  ||
  identifier(static3)
  [
  const int(1)
  ]
  ||
  identifier(static3)
  [
  const int(2)
  ]
  )
  return
  const int(3)
  ;
  char
  identifier(auto1)
  [
  const int(5)
  ]
  =
  {
  -
  const int(4)
  ,
  const int(66)
  ,
  const double(4.0)
  }
  ;
  signed
  char
  identifier(auto2)
  [
  const int(3)
  ]
  =
  {
  identifier(static1)
  [
  const int(2)
  ]
  ,
  -
  identifier(static1)
  [
  const int(0)
  ]
  }
  ;
  unsigned
  char
  identifier(auto3)
  [
  const int(2)
  ]
  =
  {
  const char('a')
  }
  ;
  if
  (
  identifier(auto1)
  [
  const int(0)
  ]
  !=
  -
  const int(4)
  ||
  identifier(auto1)
  [
  const int(1)
  ]
  !=
  const int(66)
  ||
  identifier(auto1)
  [
  const int(2)
  ]
  !=
  const int(4)
  ||
  identifier(auto1)
  [
  const int(3)
  ]
  ||
  identifier(auto1)
  [
  const int(4)
  ]
  )
  return
  const int(4)
  ;
  if
  (
  identifier(auto2)
  [
  const int(0)
  ]
  ||
  identifier(auto2)
  [
  const int(1)
  ]
  !=
  -
  const int(1)
  ||
  identifier(auto2)
  [
  const int(2)
  ]
  )
  return
  const int(5)
  ;
  if
  (
  identifier(auto3)
  [
  const int(0)
  ]
  !=
  const char('a')
  ||
  identifier(auto3)
  [
  const int(1)
  ]
  )
  return
  const int(6)
  ;
  return
  const int(0)
  ;
  }
