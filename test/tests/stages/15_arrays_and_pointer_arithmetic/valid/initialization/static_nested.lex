-- Lexing ... OK
+
+
@@ Tokens @@
List[656]:
  double
  identifier(double_arr)
  [
  const int(2)
  ]
  [
  const int(2)
  ]
  =
  {
  {
  const double(1.1)
  ,
  const double(2.2)
  }
  ,
  {
  const double(3.3)
  ,
  const double(4.4)
  }
  }
  ;
  int
  identifier(check_double_arr)
  (
  double
  (
  *
  identifier(arr)
  )
  [
  const int(2)
  ]
  )
  {
  if
  (
  identifier(arr)
  [
  const int(0)
  ]
  [
  const int(0)
  ]
  !=
  const double(1.1)
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
  const int(0)
  ]
  [
  const int(1)
  ]
  !=
  const double(2.2)
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
  const int(1)
  ]
  [
  const int(0)
  ]
  !=
  const double(3.3)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(arr)
  [
  const int(1)
  ]
  [
  const int(1)
  ]
  !=
  const double(4.4)
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
  long
  identifier(long_arr)
  [
  const int(30)
  ]
  [
  const int(50)
  ]
  [
  const int(40)
  ]
  ;
  int
  identifier(check_long_arr)
  (
  long
  (
  *
  identifier(arr)
  )
  [
  const int(50)
  ]
  [
  const int(40)
  ]
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
  const int(30)
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
  const int(50)
  ;
  identifier(j)
  =
  identifier(j)
  +
  const int(1)
  )
  {
  for
  (
  int
  identifier(k)
  =
  const int(0)
  ;
  identifier(k)
  <
  const int(40)
  ;
  identifier(k)
  =
  identifier(k)
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
  [
  identifier(j)
  ]
  [
  identifier(k)
  ]
  )
  {
  return
  const int(5)
  ;
  }
  }
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
  [
  const int(6)
  ]
  [
  const int(2)
  ]
  =
  {
  {
  {
  const double(1000.3)
  ,
  }
  ,
  {
  const unsigned int(12u)
  }
  }
  ,
  {
  {
  const int(2)
  }
  }
  }
  ;
  int
  identifier(check_ulong_arr)
  (
  unsigned
  long
  (
  *
  identifier(arr)
  )
  [
  const int(6)
  ]
  [
  const int(2)
  ]
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
  const int(4)
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
  const int(6)
  ;
  identifier(j)
  =
  identifier(j)
  +
  const int(1)
  )
  {
  for
  (
  int
  identifier(k)
  =
  const int(0)
  ;
  identifier(k)
  <
  const int(2)
  ;
  identifier(k)
  =
  identifier(k)
  +
  const int(1)
  )
  {
  int
  identifier(val)
  =
  identifier(arr)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  [
  identifier(k)
  ]
  ;
  if
  (
  identifier(i)
  ==
  const int(0)
  &&
  identifier(j)
  ==
  const int(0)
  &&
  identifier(k)
  ==
  const int(0)
  )
  {
  if
  (
  identifier(val)
  !=
  const unsigned long(1000ul)
  )
  {
  return
  const int(6)
  ;
  }
  }
  else
  if
  (
  identifier(i)
  ==
  const int(0)
  &&
  identifier(j)
  ==
  const int(1)
  &&
  identifier(k)
  ==
  const int(0)
  )
  {
  if
  (
  identifier(val)
  !=
  const unsigned long(12ul)
  )
  {
  return
  const int(7)
  ;
  }
  }
  else
  if
  (
  identifier(i)
  ==
  const int(1)
  &&
  identifier(j)
  ==
  const int(0)
  &&
  identifier(k)
  ==
  const int(0)
  )
  {
  if
  (
  identifier(val)
  !=
  const unsigned long(2ul)
  )
  {
  return
  const int(8)
  ;
  }
  }
  else
  {
  if
  (
  identifier(val)
  )
  {
  return
  const int(9)
  ;
  }
  }
  }
  }
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
  static
  double
  identifier(local_double_arr)
  [
  const int(2)
  ]
  [
  const int(2)
  ]
  =
  {
  {
  const double(1.1)
  ,
  const double(2.2)
  }
  ,
  {
  const double(3.3)
  ,
  const double(4.4)
  }
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
  static
  long
  identifier(local_long_arr)
  [
  const int(30)
  ]
  [
  const int(50)
  ]
  [
  const int(40)
  ]
  ;
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
  static
  unsigned
  long
  identifier(local_ulong_arr)
  [
  const int(4)
  ]
  [
  const int(6)
  ]
  [
  const int(2)
  ]
  =
  {
  {
  {
  const double(1000.3)
  ,
  }
  ,
  {
  const unsigned int(12u)
  }
  }
  ,
  {
  {
  const int(2)
  }
  }
  }
  ;
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
