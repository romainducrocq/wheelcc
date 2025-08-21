-- Lexing ... OK
+
+
@@ Tokens @@
List[226]:
  int
  identifier(int_gt_uint)
  (
  int
  identifier(i)
  ,
  unsigned
  int
  identifier(u)
  )
  {
  return
  identifier(i)
  >
  identifier(u)
  ;
  }
  int
  identifier(int_gt_ulong)
  (
  int
  identifier(i)
  ,
  unsigned
  long
  identifier(ul)
  )
  {
  return
  identifier(i)
  >
  identifier(ul)
  ;
  }
  int
  identifier(uint_gt_long)
  (
  unsigned
  int
  identifier(u)
  ,
  long
  identifier(l)
  )
  {
  return
  identifier(u)
  >
  identifier(l)
  ;
  }
  int
  identifier(uint_lt_ulong)
  (
  unsigned
  int
  identifier(u)
  ,
  unsigned
  long
  identifier(ul)
  )
  {
  return
  identifier(u)
  <
  identifier(ul)
  ;
  }
  int
  identifier(long_gt_ulong)
  (
  long
  identifier(l)
  ,
  unsigned
  long
  identifier(ul)
  )
  {
  return
  identifier(l)
  >
  identifier(ul)
  ;
  }
  int
  identifier(ternary_int_uint)
  (
  int
  identifier(flag)
  ,
  int
  identifier(i)
  ,
  unsigned
  int
  identifier(ui)
  )
  {
  long
  identifier(result)
  =
  identifier(flag)
  ?
  identifier(i)
  :
  identifier(ui)
  ;
  return
  (
  identifier(result)
  ==
  const long(4294967295l)
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
  identifier(int_gt_uint)
  (
  -
  const int(100)
  ,
  const unsigned int(100u)
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
  (
  identifier(int_gt_ulong)
  (
  -
  const int(1)
  ,
  const unsigned long(18446744073709551606ul)
  )
  )
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  !
  identifier(uint_gt_long)
  (
  const unsigned int(100u)
  ,
  -
  const long(100l)
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
  identifier(uint_lt_ulong)
  (
  const unsigned int(1073741824u)
  ,
  const unsigned long(34359738368ul)
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
  identifier(long_gt_ulong)
  (
  -
  const long(1l)
  ,
  const unsigned long(1000ul)
  )
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  !
  identifier(ternary_int_uint)
  (
  const int(1)
  ,
  -
  const int(1)
  ,
  const unsigned int(1u)
  )
  )
  {
  return
  const int(6)
  ;
  }
  return
  const int(0)
  ;
  }
