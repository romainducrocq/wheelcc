-- Lexing ... OK
+
+
@@ Tokens @@
List[972]:
  long
  identifier(test_sign_extend)
  (
  int
  identifier(flag)
  ,
  int
  identifier(arg)
  )
  {
  if
  (
  identifier(flag)
  )
  {
  identifier(arg)
  =
  -
  const int(1)
  ;
  }
  return
  (
  long
  )
  identifier(arg)
  ;
  }
  unsigned
  long
  identifier(test_zero_extend)
  (
  int
  identifier(flag)
  ,
  unsigned
  int
  identifier(arg)
  )
  {
  if
  (
  identifier(flag)
  )
  {
  identifier(arg)
  =
  const unsigned int(4294967295U)
  ;
  }
  return
  (
  unsigned
  long
  )
  identifier(arg)
  ;
  }
  int
  identifier(test_double_to_int)
  (
  int
  identifier(flag)
  ,
  double
  identifier(arg)
  )
  {
  if
  (
  identifier(flag)
  )
  {
  identifier(arg)
  =
  const double(225.5)
  ;
  }
  return
  (
  int
  )
  identifier(arg)
  ;
  }
  double
  identifier(test_int_to_double)
  (
  int
  identifier(flag)
  ,
  long
  identifier(arg)
  )
  {
  if
  (
  identifier(flag)
  )
  {
  identifier(arg)
  =
  const long(500000l)
  ;
  }
  return
  (
  double
  )
  identifier(arg)
  ;
  }
  unsigned
  long
  identifier(test_double_to_uint)
  (
  int
  identifier(flag)
  ,
  double
  identifier(arg)
  )
  {
  if
  (
  identifier(flag)
  )
  {
  identifier(arg)
  =
  const double(1844674407370955264.)
  ;
  }
  return
  (
  unsigned
  long
  )
  identifier(arg)
  ;
  }
  double
  identifier(test_uint_to_double)
  (
  int
  identifier(flag)
  ,
  unsigned
  int
  identifier(arg)
  )
  {
  if
  (
  identifier(flag)
  )
  {
  identifier(arg)
  =
  const unsigned int(2147483650u)
  ;
  }
  return
  (
  double
  )
  identifier(arg)
  ;
  }
  char
  identifier(test_truncate)
  (
  int
  identifier(flag)
  ,
  long
  identifier(arg)
  )
  {
  if
  (
  identifier(flag)
  )
  {
  identifier(arg)
  =
  const int(300)
  ;
  }
  return
  (
  char
  )
  identifier(arg)
  ;
  }
  double
  *
  identifier(test_add_ptr)
  (
  int
  identifier(flag)
  ,
  double
  *
  identifier(ptr)
  ,
  long
  identifier(index)
  )
  {
  static
  double
  identifier(arr)
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
  if
  (
  identifier(flag)
  ==
  const int(0)
  )
  {
  identifier(ptr)
  =
  identifier(arr)
  ;
  }
  else
  if
  (
  identifier(flag)
  ==
  const int(1)
  )
  {
  identifier(index)
  =
  const long(2l)
  ;
  }
  return
  &
  identifier(ptr)
  [
  identifier(index)
  ]
  ;
  }
  struct
  identifier(s)
  {
  int
  identifier(a)
  ;
  int
  identifier(b)
  ;
  int
  identifier(c)
  ;
  }
  ;
  int
  identifier(test_copyfromoffset)
  (
  int
  identifier(flag)
  ,
  struct
  identifier(s)
  identifier(arg)
  )
  {
  struct
  identifier(s)
  identifier(other_struct)
  =
  {
  const int(10)
  ,
  const int(9)
  ,
  const int(8)
  }
  ;
  if
  (
  identifier(flag)
  )
  {
  identifier(arg)
  =
  identifier(other_struct)
  ;
  }
  return
  identifier(arg)
  .
  identifier(b)
  ;
  }
  struct
  identifier(s)
  identifier(test_copytooffset)
  (
  int
  identifier(flag)
  ,
  int
  identifier(arg)
  )
  {
  struct
  identifier(s)
  identifier(my_struct)
  =
  {
  const int(101)
  ,
  const int(102)
  ,
  const int(103)
  }
  ;
  if
  (
  identifier(flag)
  )
  {
  identifier(arg)
  =
  -
  const int(1)
  ;
  }
  identifier(my_struct)
  .
  identifier(b)
  =
  identifier(arg)
  ;
  return
  identifier(my_struct)
  ;
  }
  void
  identifier(test_store)
  (
  int
  identifier(flag)
  ,
  long
  *
  identifier(ptr1)
  ,
  long
  *
  identifier(ptr2)
  ,
  long
  identifier(val)
  )
  {
  if
  (
  identifier(flag)
  ==
  const int(1)
  )
  {
  identifier(ptr1)
  =
  identifier(ptr2)
  ;
  }
  if
  (
  identifier(flag)
  ==
  const int(2)
  )
  {
  identifier(val)
  =
  const long(77l)
  ;
  }
  *
  identifier(ptr1)
  =
  identifier(val)
  ;
  }
  int
  identifier(test_load)
  (
  int
  identifier(flag)
  ,
  int
  *
  identifier(ptr1)
  ,
  int
  *
  identifier(ptr2)
  )
  {
  if
  (
  identifier(flag)
  )
  {
  identifier(ptr1)
  =
  identifier(ptr2)
  ;
  }
  return
  *
  identifier(ptr1)
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
  identifier(test_sign_extend)
  (
  const int(0)
  ,
  -
  const int(5)
  )
  !=
  -
  const long(5l)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(test_sign_extend)
  (
  const int(1)
  ,
  -
  const int(5)
  )
  !=
  -
  const long(1l)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(test_zero_extend)
  (
  const int(0)
  ,
  const unsigned int(100000u)
  )
  !=
  const unsigned long(100000ul)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(test_zero_extend)
  (
  const int(1)
  ,
  const unsigned int(100000u)
  )
  !=
  const unsigned long(4294967295UL)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(test_double_to_int)
  (
  const int(0)
  ,
  const double(1000.5)
  )
  !=
  const int(1000)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(test_double_to_int)
  (
  const int(1)
  ,
  const double(1000.5)
  )
  !=
  const int(225)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(test_int_to_double)
  (
  const int(0)
  ,
  const int(100)
  )
  !=
  const double(100.0)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(test_int_to_double)
  (
  const int(1)
  ,
  const int(100)
  )
  !=
  const double(500000.0)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(test_double_to_uint)
  (
  const int(0)
  ,
  const double(1234567.8)
  )
  !=
  const unsigned int(1234567u)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(test_double_to_uint)
  (
  const int(1)
  ,
  const double(1234567.8)
  )
  !=
  const unsigned int(1844674407370955264u)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  identifier(test_uint_to_double)
  (
  const int(0)
  ,
  const unsigned int(4294967000U)
  )
  !=
  const double(4294967000.)
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  identifier(test_uint_to_double)
  (
  const int(1)
  ,
  const unsigned int(4294967000U)
  )
  !=
  const unsigned int(2147483650u)
  )
  {
  return
  const int(12)
  ;
  }
  if
  (
  identifier(test_truncate)
  (
  const int(0)
  ,
  const int(500)
  )
  !=
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
  identifier(test_truncate)
  (
  const int(1)
  ,
  const int(500)
  )
  !=
  const int(44)
  )
  {
  return
  const int(14)
  ;
  }
  double
  identifier(arr)
  [
  const int(3)
  ]
  =
  {
  const double(4.0)
  ,
  const double(5.0)
  ,
  const double(6.0)
  }
  ;
  if
  (
  *
  identifier(test_add_ptr)
  (
  const int(0)
  ,
  identifier(arr)
  ,
  const int(1)
  )
  !=
  const double(2.0)
  )
  {
  return
  const int(15)
  ;
  }
  if
  (
  *
  identifier(test_add_ptr)
  (
  const int(1)
  ,
  identifier(arr)
  ,
  const int(1)
  )
  !=
  const double(6.0)
  )
  {
  return
  const int(16)
  ;
  }
  if
  (
  *
  identifier(test_add_ptr)
  (
  const int(2)
  ,
  identifier(arr)
  ,
  const int(1)
  )
  !=
  const double(5.0)
  )
  {
  return
  const int(17)
  ;
  }
  struct
  identifier(s)
  identifier(strct)
  =
  {
  const int(20)
  ,
  const int(21)
  ,
  const int(22)
  }
  ;
  if
  (
  identifier(test_copyfromoffset)
  (
  const int(0)
  ,
  identifier(strct)
  )
  !=
  const int(21)
  )
  {
  return
  const int(18)
  ;
  }
  if
  (
  identifier(test_copyfromoffset)
  (
  const int(1)
  ,
  identifier(strct)
  )
  !=
  const int(9)
  )
  {
  return
  const int(19)
  ;
  }
  if
  (
  identifier(test_copytooffset)
  (
  const int(0)
  ,
  -
  const int(10)
  )
  .
  identifier(b)
  !=
  -
  const int(10)
  )
  {
  return
  const int(20)
  ;
  }
  if
  (
  identifier(test_copytooffset)
  (
  const int(1)
  ,
  -
  const int(10)
  )
  .
  identifier(b)
  !=
  -
  const int(1)
  )
  {
  return
  const int(21)
  ;
  }
  long
  identifier(l1)
  =
  const long(0l)
  ;
  long
  identifier(l2)
  =
  const long(0l)
  ;
  identifier(test_store)
  (
  const int(0)
  ,
  &
  identifier(l1)
  ,
  &
  identifier(l2)
  ,
  const long(5l)
  )
  ;
  if
  (
  identifier(l1)
  !=
  const long(5l)
  ||
  identifier(l2)
  !=
  const long(0l)
  )
  {
  return
  const int(22)
  ;
  }
  identifier(test_store)
  (
  const int(1)
  ,
  &
  identifier(l1)
  ,
  &
  identifier(l2)
  ,
  const long(6l)
  )
  ;
  if
  (
  identifier(l1)
  !=
  const long(5l)
  ||
  identifier(l2)
  !=
  const long(6l)
  )
  {
  return
  const int(23)
  ;
  }
  identifier(test_store)
  (
  const int(2)
  ,
  &
  identifier(l1)
  ,
  &
  identifier(l2)
  ,
  const long(5l)
  )
  ;
  if
  (
  identifier(l1)
  !=
  const long(77l)
  ||
  identifier(l2)
  !=
  const long(6l)
  )
  {
  return
  const int(24)
  ;
  }
  int
  identifier(i1)
  =
  const int(2)
  ;
  int
  identifier(i2)
  =
  const int(3)
  ;
  if
  (
  identifier(test_load)
  (
  const int(0)
  ,
  &
  identifier(i1)
  ,
  &
  identifier(i2)
  )
  !=
  const int(2)
  )
  {
  return
  const int(25)
  ;
  }
  if
  (
  identifier(test_load)
  (
  const int(1)
  ,
  &
  identifier(i1)
  ,
  &
  identifier(i2)
  )
  !=
  const int(3)
  )
  {
  return
  const int(26)
  ;
  }
  return
  const int(0)
  ;
  }
