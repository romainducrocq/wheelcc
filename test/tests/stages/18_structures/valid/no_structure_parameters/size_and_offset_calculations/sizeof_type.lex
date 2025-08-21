-- Lexing ... OK
+
+
@@ Tokens @@
List[326]:
  struct
  identifier(eight_bytes)
  {
  int
  identifier(i)
  ;
  char
  identifier(c)
  ;
  }
  ;
  struct
  identifier(two_bytes)
  {
  char
  identifier(arr)
  [
  const int(2)
  ]
  ;
  }
  ;
  struct
  identifier(three_bytes)
  {
  char
  identifier(arr)
  [
  const int(3)
  ]
  ;
  }
  ;
  struct
  identifier(sixteen_bytes)
  {
  struct
  identifier(eight_bytes)
  identifier(eight)
  ;
  struct
  identifier(two_bytes)
  identifier(two)
  ;
  struct
  identifier(three_bytes)
  identifier(three)
  ;
  }
  ;
  struct
  identifier(seven_bytes)
  {
  struct
  identifier(two_bytes)
  identifier(two)
  ;
  struct
  identifier(three_bytes)
  identifier(three)
  ;
  struct
  identifier(two_bytes)
  identifier(two2)
  ;
  }
  ;
  struct
  identifier(twentyfour_bytes)
  {
  struct
  identifier(seven_bytes)
  identifier(seven)
  ;
  struct
  identifier(sixteen_bytes)
  identifier(sixteen)
  ;
  }
  ;
  struct
  identifier(twenty_bytes)
  {
  struct
  identifier(sixteen_bytes)
  identifier(sixteen)
  ;
  struct
  identifier(two_bytes)
  identifier(two)
  ;
  }
  ;
  struct
  identifier(wonky)
  {
  char
  identifier(arr)
  [
  const int(19)
  ]
  ;
  }
  ;
  struct
  identifier(internal_padding)
  {
  char
  identifier(c)
  ;
  double
  identifier(d)
  ;
  }
  ;
  struct
  identifier(contains_struct_array)
  {
  char
  identifier(c)
  ;
  struct
  identifier(eight_bytes)
  identifier(struct_array)
  [
  const int(3)
  ]
  ;
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
  sizeof
  (
  struct
  identifier(eight_bytes)
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
  struct
  identifier(two_bytes)
  )
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
  sizeof
  (
  struct
  identifier(three_bytes)
  )
  !=
  const int(3)
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
  struct
  identifier(sixteen_bytes)
  )
  !=
  const int(16)
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
  struct
  identifier(seven_bytes)
  )
  !=
  const int(7)
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
  struct
  identifier(twentyfour_bytes)
  )
  !=
  const int(24)
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
  struct
  identifier(twenty_bytes)
  )
  !=
  const int(20)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  sizeof
  (
  struct
  identifier(wonky)
  )
  !=
  const int(19)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  sizeof
  (
  struct
  identifier(internal_padding)
  )
  !=
  const int(16)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  sizeof
  (
  struct
  identifier(contains_struct_array)
  )
  !=
  const int(28)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  sizeof
  (
  struct
  identifier(internal_padding)
  [
  const int(4)
  ]
  )
  !=
  const int(64)
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  sizeof
  (
  struct
  identifier(wonky)
  [
  const int(2)
  ]
  )
  !=
  const int(38)
  )
  {
  return
  const int(12)
  ;
  }
  return
  const int(0)
  ;
  }
