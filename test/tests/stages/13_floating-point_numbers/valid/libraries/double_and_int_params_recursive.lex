-- Lexing ... OK
+
+
@@ Tokens @@
List[416]:
  int
  identifier(fun)
  (
  int
  identifier(i1)
  ,
  double
  identifier(d1)
  ,
  int
  identifier(i2)
  ,
  double
  identifier(d2)
  ,
  int
  identifier(i3)
  ,
  double
  identifier(d3)
  ,
  int
  identifier(i4)
  ,
  double
  identifier(d4)
  ,
  int
  identifier(i5)
  ,
  double
  identifier(d5)
  ,
  int
  identifier(i6)
  ,
  double
  identifier(d6)
  ,
  int
  identifier(i7)
  ,
  double
  identifier(d7)
  ,
  int
  identifier(i8)
  ,
  double
  identifier(d8)
  ,
  int
  identifier(i9)
  ,
  double
  identifier(d9)
  )
  {
  if
  (
  identifier(i1)
  !=
  identifier(d9)
  )
  {
  int
  identifier(call1)
  =
  identifier(fun)
  (
  identifier(i1)
  +
  const int(1)
  ,
  identifier(d1)
  ,
  identifier(i2)
  +
  const int(1)
  ,
  identifier(d2)
  ,
  identifier(i3)
  +
  const int(1)
  ,
  identifier(d3)
  ,
  identifier(i4)
  +
  const int(1)
  ,
  identifier(d4)
  ,
  identifier(i5)
  +
  const int(1)
  ,
  identifier(d5)
  ,
  identifier(i6)
  +
  const int(1)
  ,
  identifier(d6)
  ,
  identifier(i7)
  +
  const int(1)
  ,
  identifier(d7)
  ,
  identifier(i8)
  +
  const int(1)
  ,
  identifier(d8)
  ,
  identifier(i9)
  +
  const int(1)
  ,
  identifier(d9)
  )
  ;
  int
  identifier(call2)
  =
  identifier(fun)
  (
  identifier(i1)
  ,
  identifier(d1)
  -
  const int(1)
  ,
  identifier(i2)
  ,
  identifier(d2)
  -
  const int(1)
  ,
  identifier(i3)
  ,
  identifier(d3)
  -
  const int(1)
  ,
  identifier(i4)
  ,
  identifier(d4)
  -
  const int(1)
  ,
  identifier(i5)
  ,
  identifier(d5)
  -
  const int(1)
  ,
  identifier(i6)
  ,
  identifier(d6)
  -
  const int(1)
  ,
  identifier(i7)
  ,
  identifier(d7)
  -
  const int(1)
  ,
  identifier(i8)
  ,
  identifier(d8)
  -
  const int(1)
  ,
  identifier(i9)
  ,
  identifier(d9)
  -
  const int(1)
  )
  ;
  if
  (
  identifier(call1)
  )
  {
  return
  identifier(call1)
  ;
  }
  if
  (
  identifier(call2)
  )
  {
  return
  identifier(call2)
  ;
  }
  }
  if
  (
  identifier(i2)
  !=
  identifier(i1)
  +
  const int(2)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(i3)
  !=
  identifier(i1)
  +
  const int(4)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(i4)
  !=
  identifier(i1)
  +
  const int(6)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(i5)
  !=
  identifier(i1)
  +
  const int(8)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(i6)
  !=
  identifier(i1)
  +
  const int(10)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(i7)
  !=
  identifier(i1)
  +
  const int(12)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(i8)
  !=
  identifier(i1)
  +
  const int(14)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(i9)
  !=
  identifier(i1)
  +
  const int(16)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(d1)
  !=
  identifier(d9)
  -
  const int(16)
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  identifier(d2)
  !=
  identifier(d9)
  -
  const int(14)
  )
  {
  return
  const int(12)
  ;
  }
  if
  (
  identifier(d3)
  !=
  identifier(d9)
  -
  const int(12)
  )
  {
  return
  const int(13)
  ;
  }
  if
  (
  identifier(d4)
  !=
  identifier(d9)
  -
  const int(10)
  )
  {
  return
  const int(14)
  ;
  }
  if
  (
  identifier(d5)
  !=
  identifier(d9)
  -
  const int(8)
  )
  {
  return
  const int(15)
  ;
  }
  if
  (
  identifier(d6)
  !=
  identifier(d9)
  -
  const int(6)
  )
  {
  return
  const int(16)
  ;
  }
  if
  (
  identifier(d7)
  !=
  identifier(d9)
  -
  const int(4)
  )
  {
  return
  const int(17)
  ;
  }
  if
  (
  identifier(d8)
  !=
  identifier(d9)
  -
  const int(2)
  )
  {
  return
  const int(18)
  ;
  }
  return
  const int(0)
  ;
  }
