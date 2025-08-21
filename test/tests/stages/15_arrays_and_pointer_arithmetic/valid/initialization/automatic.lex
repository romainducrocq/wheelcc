-- Lexing ... OK
+
+
@@ Tokens @@
List[461]:
  int
  identifier(test_simple)
  (
  void
  )
  {
  unsigned
  long
  identifier(arr)
  [
  const int(3)
  ]
  =
  {
  const unsigned long(18446744073709551615UL)
  ,
  const unsigned long(9223372036854775807UL)
  ,
  const unsigned long(100ul)
  }
  ;
  return
  (
  identifier(arr)
  [
  const int(0)
  ]
  ==
  const unsigned long(18446744073709551615UL)
  &&
  identifier(arr)
  [
  const int(1)
  ]
  ==
  const unsigned long(9223372036854775807UL)
  &&
  identifier(arr)
  [
  const int(2)
  ]
  ==
  const unsigned long(100ul)
  )
  ;
  }
  int
  identifier(test_partial)
  (
  void
  )
  {
  double
  identifier(arr)
  [
  const int(5)
  ]
  =
  {
  const double(1.0)
  ,
  const double(123e4)
  }
  ;
  return
  (
  identifier(arr)
  [
  const int(0)
  ]
  ==
  const double(1.0)
  &&
  identifier(arr)
  [
  const int(1)
  ]
  ==
  const double(123e4)
  &&
  !
  identifier(arr)
  [
  const int(2)
  ]
  &&
  !
  identifier(arr)
  [
  const int(3)
  ]
  &&
  !
  identifier(arr)
  [
  const int(4)
  ]
  )
  ;
  }
  int
  identifier(test_non_constant)
  (
  long
  identifier(negative_7billion)
  ,
  int
  *
  identifier(ptr)
  )
  {
  *
  identifier(ptr)
  =
  const int(1)
  ;
  extern
  int
  identifier(three)
  (
  void
  )
  ;
  long
  identifier(var)
  =
  identifier(negative_7billion)
  *
  identifier(three)
  (
  )
  ;
  long
  identifier(arr)
  [
  const int(5)
  ]
  =
  {
  identifier(negative_7billion)
  ,
  identifier(three)
  (
  )
  *
  const long(7l)
  ,
  -
  (
  long
  )
  *
  identifier(ptr)
  ,
  identifier(var)
  +
  (
  identifier(negative_7billion)
  ?
  const int(2)
  :
  const int(3)
  )
  }
  ;
  return
  (
  identifier(arr)
  [
  const int(0)
  ]
  ==
  -
  const int(7000000000)
  &&
  identifier(arr)
  [
  const int(1)
  ]
  ==
  const long(21l)
  &&
  identifier(arr)
  [
  const int(2)
  ]
  ==
  -
  const long(1l)
  &&
  identifier(arr)
  [
  const int(3)
  ]
  ==
  -
  const long(20999999998l)
  &&
  identifier(arr)
  [
  const int(4)
  ]
  ==
  const long(0l)
  )
  ;
  }
  int
  identifier(three)
  (
  void
  )
  {
  return
  const int(3)
  ;
  }
  long
  identifier(global_one)
  =
  const long(1l)
  ;
  int
  identifier(test_type_conversion)
  (
  int
  *
  identifier(ptr)
  )
  {
  *
  identifier(ptr)
  =
  -
  const int(100)
  ;
  unsigned
  long
  identifier(arr)
  [
  const int(4)
  ]
  =
  {
  const double(3458764513821589504.0)
  ,
  *
  identifier(ptr)
  ,
  (
  unsigned
  int
  )
  const unsigned long(18446744073709551615UL)
  ,
  -
  identifier(global_one)
  }
  ;
  return
  (
  identifier(arr)
  [
  const int(0)
  ]
  ==
  const unsigned long(3458764513821589504ul)
  &&
  identifier(arr)
  [
  const int(1)
  ]
  ==
  const unsigned long(18446744073709551516ul)
  &&
  identifier(arr)
  [
  const int(2)
  ]
  ==
  const unsigned int(4294967295U)
  &&
  identifier(arr)
  [
  const int(3)
  ]
  ==
  const unsigned long(18446744073709551615UL)
  )
  ;
  }
  int
  identifier(test_preserve_stack)
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
  identifier(arr)
  [
  const int(3)
  ]
  =
  {
  identifier(global_one)
  *
  const long(2l)
  ,
  identifier(global_one)
  +
  identifier(three)
  (
  )
  }
  ;
  unsigned
  int
  identifier(u)
  =
  const int(2684366905)
  ;
  if
  (
  identifier(i)
  !=
  -
  const int(1)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(u)
  !=
  const int(2684366905)
  )
  {
  return
  const int(0)
  ;
  }
  return
  (
  identifier(arr)
  [
  const int(0)
  ]
  ==
  const int(2)
  &&
  identifier(arr)
  [
  const int(1)
  ]
  ==
  const int(4)
  &&
  !
  identifier(arr)
  [
  const int(2)
  ]
  )
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  !
  identifier(test_simple)
  (
  )
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  !
  identifier(test_partial)
  (
  )
  )
  {
  return
  const int(2)
  ;
  }
  long
  identifier(negative_seven_billion)
  =
  -
  const long(7000000000l)
  ;
  int
  identifier(i)
  =
  const int(0)
  ;
  if
  (
  !
  identifier(test_non_constant)
  (
  identifier(negative_seven_billion)
  ,
  &
  identifier(i)
  )
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  !
  identifier(test_type_conversion)
  (
  &
  identifier(i)
  )
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  !
  identifier(test_preserve_stack)
  (
  )
  )
  {
  return
  const int(5)
  ;
  }
  return
  const int(0)
  ;
  }
