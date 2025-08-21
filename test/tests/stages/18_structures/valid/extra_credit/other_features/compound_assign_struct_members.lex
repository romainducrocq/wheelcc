-- Lexing ... OK
+
+
@@ Tokens @@
List[542]:
  struct
  identifier(inner)
  {
  double
  identifier(a)
  ;
  char
  identifier(b)
  ;
  int
  *
  identifier(ptr)
  ;
  }
  ;
  struct
  identifier(outer)
  {
  unsigned
  long
  identifier(l)
  ;
  struct
  identifier(inner)
  *
  identifier(in_ptr)
  ;
  struct
  identifier(inner)
  identifier(in_array)
  [
  const int(4)
  ]
  ;
  int
  identifier(bar)
  ;
  }
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
  -
  const int(1)
  ;
  int
  identifier(i2)
  =
  -
  const int(2)
  ;
  struct
  identifier(inner)
  identifier(si)
  =
  {
  const double(150.)
  ,
  -
  const int(12)
  ,
  &
  identifier(i)
  }
  ;
  struct
  identifier(outer)
  identifier(o)
  =
  {
  const unsigned long(18446744073709551615UL)
  ,
  &
  identifier(si)
  ,
  {
  identifier(si)
  ,
  {
  -
  const double(20e20)
  ,
  const int(120)
  ,
  const int(0)
  }
  ,
  {
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  }
  ,
  {
  const int(1)
  ,
  const int(1)
  ,
  &
  identifier(i2)
  }
  }
  ,
  const int(2000)
  }
  ;
  identifier(si)
  .
  identifier(a)
  +=
  const int(10)
  ;
  if
  (
  identifier(si)
  .
  identifier(a)
  !=
  const int(160)
  )
  {
  return
  const int(1)
  ;
  }
  identifier(o)
  .
  identifier(in_array)
  [
  const int(0)
  ]
  .
  identifier(b)
  -=
  const int(460)
  ;
  if
  (
  identifier(o)
  .
  identifier(in_array)
  [
  const int(0)
  ]
  .
  identifier(b)
  !=
  const int(40)
  )
  {
  return
  const int(2)
  ;
  }
  identifier(o)
  .
  identifier(in_array)
  [
  const int(1)
  ]
  .
  identifier(a)
  *=
  -
  const int(4)
  ;
  if
  (
  identifier(o)
  .
  identifier(in_array)
  [
  const int(1)
  ]
  .
  identifier(a)
  !=
  const double(80e20)
  )
  {
  return
  const int(4)
  ;
  }
  identifier(o)
  .
  identifier(in_ptr)
  ->
  identifier(a)
  /=
  const int(5)
  ;
  if
  (
  identifier(si)
  .
  identifier(a)
  !=
  const int(32)
  )
  {
  return
  const int(5)
  ;
  }
  (
  &
  identifier(o)
  )
  ->
  identifier(l)
  %=
  identifier(o)
  .
  identifier(bar)
  ;
  if
  (
  identifier(o)
  .
  identifier(l)
  !=
  const int(1615)
  )
  {
  return
  const int(6)
  ;
  }
  identifier(o)
  .
  identifier(in_ptr)
  =
  identifier(o)
  .
  identifier(in_array)
  ;
  if
  (
  (
  identifier(o)
  .
  identifier(in_ptr)
  +=
  const int(3)
  )
  ->
  identifier(a)
  !=
  const int(1)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  *
  identifier(o)
  .
  identifier(in_ptr)
  ->
  identifier(ptr)
  !=
  -
  const int(2)
  )
  {
  return
  const int(8)
  ;
  }
  identifier(o)
  .
  identifier(in_ptr)
  -=
  const unsigned int(1u)
  ;
  if
  (
  identifier(o)
  .
  identifier(in_ptr)
  ->
  identifier(a)
  ||
  identifier(o)
  .
  identifier(in_ptr)
  ->
  identifier(b)
  ||
  identifier(o)
  .
  identifier(in_ptr)
  ->
  identifier(ptr)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(si)
  .
  identifier(a)
  !=
  const int(32)
  ||
  identifier(si)
  .
  identifier(b)
  !=
  -
  const int(12)
  ||
  identifier(si)
  .
  identifier(ptr)
  !=
  &
  identifier(i)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  identifier(o)
  .
  identifier(l)
  !=
  const int(1615)
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  identifier(o)
  .
  identifier(in_ptr)
  !=
  &
  identifier(o)
  .
  identifier(in_array)
  [
  const int(2)
  ]
  )
  {
  return
  const int(12)
  ;
  }
  if
  (
  identifier(o)
  .
  identifier(in_array)
  [
  const int(0)
  ]
  .
  identifier(a)
  !=
  const double(150.)
  ||
  identifier(o)
  .
  identifier(in_array)
  [
  const int(0)
  ]
  .
  identifier(b)
  !=
  const int(40)
  ||
  identifier(o)
  .
  identifier(in_array)
  [
  const int(0)
  ]
  .
  identifier(ptr)
  !=
  &
  identifier(i)
  )
  {
  return
  const int(13)
  ;
  }
  if
  (
  identifier(o)
  .
  identifier(in_array)
  [
  const int(1)
  ]
  .
  identifier(a)
  !=
  const double(80e20)
  ||
  identifier(o)
  .
  identifier(in_array)
  [
  const int(1)
  ]
  .
  identifier(b)
  !=
  const int(120)
  ||
  identifier(o)
  .
  identifier(in_array)
  [
  const int(1)
  ]
  .
  identifier(ptr)
  )
  {
  return
  const int(14)
  ;
  }
  if
  (
  identifier(o)
  .
  identifier(in_array)
  [
  const int(2)
  ]
  .
  identifier(a)
  ||
  identifier(o)
  .
  identifier(in_array)
  [
  const int(2)
  ]
  .
  identifier(b)
  ||
  identifier(o)
  .
  identifier(in_array)
  [
  const int(2)
  ]
  .
  identifier(ptr)
  )
  {
  return
  const int(15)
  ;
  }
  if
  (
  identifier(o)
  .
  identifier(in_array)
  [
  const int(3)
  ]
  .
  identifier(a)
  !=
  const int(1)
  ||
  identifier(o)
  .
  identifier(in_array)
  [
  const int(3)
  ]
  .
  identifier(b)
  !=
  const int(1)
  ||
  identifier(o)
  .
  identifier(in_array)
  [
  const int(3)
  ]
  .
  identifier(ptr)
  !=
  &
  identifier(i2)
  )
  {
  return
  const int(16)
  ;
  }
  if
  (
  identifier(o)
  .
  identifier(bar)
  !=
  const int(2000)
  )
  {
  return
  const int(17)
  ;
  }
  return
  const int(0)
  ;
  }
