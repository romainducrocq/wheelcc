-- Lexing ... OK
+
+
@@ Tokens @@
List[495]:
  int
  identifier(i)
  =
  const int(4)
  ;
  int
  identifier(int_array)
  (
  void
  )
  {
  int
  identifier(arr)
  [
  const int(6)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  ,
  const int(4)
  ,
  const int(5)
  ,
  const int(6)
  }
  ;
  int
  *
  identifier(ptr)
  =
  identifier(arr)
  ;
  if
  (
  *
  (
  identifier(ptr)
  +=
  const int(5)
  )
  !=
  const int(6)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(ptr)
  [
  const int(0)
  ]
  !=
  const int(6)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(ptr)
  !=
  identifier(arr)
  +
  const int(5)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  *
  (
  identifier(ptr)
  -=
  const int(3)
  )
  !=
  const int(3)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(ptr)
  [
  const int(0)
  ]
  !=
  const int(3)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(ptr)
  !=
  identifier(arr)
  +
  const int(2)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  (
  identifier(ptr)
  +=
  identifier(i)
  -
  const int(1)
  )
  !=
  identifier(arr)
  +
  const int(5)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  *
  identifier(ptr)
  !=
  const int(6)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  (
  identifier(ptr)
  -=
  (
  const unsigned int(4294967295U)
  +
  identifier(i)
  )
  )
  !=
  identifier(arr)
  +
  const int(2)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  *
  identifier(ptr)
  !=
  const int(3)
  )
  {
  return
  const int(10)
  ;
  }
  long
  identifier(l)
  =
  const long(9223372036854775807l)
  ;
  if
  (
  (
  identifier(ptr)
  +=
  identifier(l)
  -
  const long(9223372036854775806l)
  )
  !=
  identifier(arr)
  +
  const int(3)
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  *
  identifier(ptr)
  !=
  const int(4)
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
  int
  identifier(double_array)
  (
  void
  )
  {
  static
  double
  identifier(arr)
  [
  const int(6)
  ]
  =
  {
  const double(1.0)
  ,
  const double(2.0)
  ,
  const double(3.0)
  ,
  const double(4.0)
  ,
  const double(5.0)
  ,
  const double(6.0)
  }
  ;
  double
  *
  identifier(ptr)
  =
  identifier(arr)
  ;
  if
  (
  *
  (
  identifier(ptr)
  +=
  const int(5)
  )
  !=
  const int(6)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(ptr)
  [
  const int(0)
  ]
  !=
  const int(6)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(ptr)
  !=
  identifier(arr)
  +
  const int(5)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  *
  (
  identifier(ptr)
  -=
  const int(3)
  )
  !=
  const int(3)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(ptr)
  [
  const int(0)
  ]
  !=
  const int(3)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(ptr)
  !=
  identifier(arr)
  +
  const int(2)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  (
  identifier(ptr)
  +=
  identifier(i)
  -
  const int(1)
  )
  !=
  identifier(arr)
  +
  const int(5)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  *
  identifier(ptr)
  !=
  const int(6)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  (
  identifier(ptr)
  -=
  (
  const unsigned int(4294967295U)
  +
  identifier(i)
  )
  )
  !=
  identifier(arr)
  +
  const int(2)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  *
  identifier(ptr)
  !=
  const int(3)
  )
  {
  return
  const int(10)
  ;
  }
  long
  identifier(l)
  =
  const long(9223372036854775807l)
  ;
  if
  (
  (
  identifier(ptr)
  +=
  identifier(l)
  -
  const long(9223372036854775806l)
  )
  !=
  identifier(arr)
  +
  const int(3)
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  *
  identifier(ptr)
  !=
  const int(4)
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
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(result)
  ;
  if
  (
  (
  identifier(result)
  =
  identifier(int_array)
  (
  )
  )
  )
  {
  return
  identifier(result)
  ;
  }
  if
  (
  (
  identifier(result)
  =
  identifier(double_array)
  (
  )
  )
  )
  {
  return
  identifier(result)
  +
  const int(12)
  ;
  }
  return
  const int(0)
  ;
  }
