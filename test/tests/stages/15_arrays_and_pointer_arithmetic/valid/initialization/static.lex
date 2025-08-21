-- Lexing ... OK
+
+
@@ Tokens @@
List[528]:
  double
  identifier(double_arr)
  [
  const int(3)
  ]
  =
  {
  const double(1.0)
  ,
  const double(2.0)
  ,
  const double(3.0)
  }
  ;
  int
  identifier(check_double_arr)
  (
  double
  *
  identifier(arr)
  )
  {
  if
  (
  identifier(arr)
  [
  const int(0)
  ]
  !=
  const double(1.0)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(arr)
  [
  const int(1)
  ]
  !=
  const double(2.0)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(arr)
  [
  const int(2)
  ]
  !=
  const double(3.0)
  )
  {
  return
  const int(3)
  ;
  }
  return
  const int(0)
  ;
  }
  unsigned
  identifier(uint_arr)
  [
  const int(5)
  ]
  =
  {
  const unsigned int(1u)
  ,
  const unsigned int(0u)
  ,
  const unsigned int(2147497230u)
  ,
  }
  ;
  int
  identifier(check_uint_arr)
  (
  unsigned
  *
  identifier(arr)
  )
  {
  if
  (
  identifier(arr)
  [
  const int(0)
  ]
  !=
  const unsigned int(1u)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(arr)
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
  identifier(arr)
  [
  const int(2)
  ]
  !=
  const unsigned int(2147497230u)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(arr)
  [
  const int(3)
  ]
  ||
  identifier(arr)
  [
  const int(4)
  ]
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
  long
  identifier(long_arr)
  [
  const int(1000)
  ]
  ;
  int
  identifier(check_long_arr)
  (
  long
  *
  identifier(arr)
  )
  {
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(1000)
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
  identifier(arr)
  [
  identifier(i)
  ]
  )
  {
  return
  const int(8)
  ;
  }
  }
  return
  const int(0)
  ;
  }
  unsigned
  long
  identifier(ulong_arr)
  [
  const int(4)
  ]
  =
  {
  const double(100.0)
  ,
  const int(11)
  ,
  const long(12345l)
  ,
  const unsigned int(4294967295U)
  }
  ;
  int
  identifier(check_ulong_arr)
  (
  unsigned
  long
  *
  identifier(arr)
  )
  {
  if
  (
  identifier(arr)
  [
  const int(0)
  ]
  !=
  const unsigned long(100ul)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(arr)
  [
  const int(1)
  ]
  !=
  const unsigned long(11ul)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  identifier(arr)
  [
  const int(2)
  ]
  !=
  const unsigned long(12345ul)
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  identifier(arr)
  [
  const int(3)
  ]
  !=
  const unsigned long(4294967295Ul)
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
  identifier(test_global)
  (
  void
  )
  {
  int
  identifier(check)
  =
  identifier(check_double_arr)
  (
  identifier(double_arr)
  )
  ;
  if
  (
  identifier(check)
  )
  {
  return
  identifier(check)
  ;
  }
  identifier(check)
  =
  identifier(check_uint_arr)
  (
  identifier(uint_arr)
  )
  ;
  if
  (
  identifier(check)
  )
  {
  return
  identifier(check)
  ;
  }
  identifier(check)
  =
  identifier(check_long_arr)
  (
  identifier(long_arr)
  )
  ;
  if
  (
  identifier(check)
  )
  {
  return
  identifier(check)
  ;
  }
  identifier(check)
  =
  identifier(check_ulong_arr)
  (
  identifier(ulong_arr)
  )
  ;
  if
  (
  identifier(check)
  )
  {
  return
  identifier(check)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(test_local)
  (
  void
  )
  {
  double
  identifier(local_double_arr)
  [
  const int(3)
  ]
  =
  {
  const double(1.0)
  ,
  const double(2.0)
  ,
  const double(3.0)
  }
  ;
  static
  unsigned
  identifier(local_uint_arr)
  [
  const int(5)
  ]
  =
  {
  const unsigned int(1u)
  ,
  const unsigned int(0u)
  ,
  const unsigned int(2147497230u)
  ,
  }
  ;
  static
  long
  identifier(local_long_arr)
  [
  const int(1000)
  ]
  ;
  static
  unsigned
  long
  identifier(local_ulong_arr)
  [
  const int(4)
  ]
  =
  {
  const double(100.0)
  ,
  const int(11)
  ,
  const long(12345l)
  ,
  const unsigned int(4294967295U)
  }
  ;
  int
  identifier(check)
  =
  identifier(check_double_arr)
  (
  identifier(local_double_arr)
  )
  ;
  if
  (
  identifier(check)
  )
  {
  return
  const int(100)
  +
  identifier(check)
  ;
  }
  identifier(check)
  =
  identifier(check_uint_arr)
  (
  identifier(local_uint_arr)
  )
  ;
  if
  (
  identifier(check)
  )
  {
  return
  const int(100)
  +
  identifier(check)
  ;
  }
  identifier(check)
  =
  identifier(check_long_arr)
  (
  identifier(local_long_arr)
  )
  ;
  if
  (
  identifier(check)
  )
  {
  return
  const int(100)
  +
  identifier(check)
  ;
  }
  identifier(check)
  =
  identifier(check_ulong_arr)
  (
  identifier(local_ulong_arr)
  )
  ;
  if
  (
  identifier(check)
  )
  {
  return
  const int(100)
  +
  identifier(check)
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
  identifier(check)
  =
  identifier(test_global)
  (
  )
  ;
  if
  (
  identifier(check)
  )
  {
  return
  identifier(check)
  ;
  }
  return
  identifier(test_local)
  (
  )
  ;
  }
