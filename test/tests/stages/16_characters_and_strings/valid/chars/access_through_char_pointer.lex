-- Lexing ... OK
+
+
@@ Tokens @@
List[229]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(100)
  ;
  char
  *
  identifier(byte_ptr)
  =
  (
  char
  *
  )
  &
  identifier(x)
  ;
  if
  (
  identifier(byte_ptr)
  [
  const int(0)
  ]
  !=
  const int(100)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(byte_ptr)
  [
  const int(1)
  ]
  ||
  identifier(byte_ptr)
  [
  const int(2)
  ]
  ||
  identifier(byte_ptr)
  [
  const int(3)
  ]
  )
  {
  return
  const int(2)
  ;
  }
  double
  identifier(d)
  =
  -
  const double(0.0)
  ;
  identifier(byte_ptr)
  =
  (
  char
  *
  )
  &
  identifier(d)
  ;
  if
  (
  identifier(byte_ptr)
  [
  const int(7)
  ]
  !=
  -
  const int(128)
  )
  {
  return
  const int(3)
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
  {
  if
  (
  identifier(byte_ptr)
  [
  identifier(i)
  ]
  )
  {
  return
  const int(4)
  ;
  }
  }
  unsigned
  int
  identifier(array)
  [
  const int(3)
  ]
  [
  const int(2)
  ]
  [
  const int(1)
  ]
  =
  {
  {
  {
  -
  const int(1)
  }
  ,
  {
  -
  const int(1)
  }
  }
  ,
  {
  {
  -
  const int(1)
  }
  ,
  {
  -
  const int(1)
  }
  }
  ,
  {
  {
  const unsigned int(4294901760u)
  }
  }
  }
  ;
  identifier(byte_ptr)
  =
  (
  char
  *
  )
  identifier(array)
  ;
  identifier(byte_ptr)
  =
  identifier(byte_ptr)
  +
  const int(16)
  ;
  if
  (
  identifier(byte_ptr)
  [
  const int(0)
  ]
  ||
  identifier(byte_ptr)
  [
  const int(1)
  ]
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(byte_ptr)
  [
  const int(2)
  ]
  !=
  -
  const int(1)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(byte_ptr)
  [
  const int(3)
  ]
  !=
  -
  const int(1)
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
