-- Lexing ... OK
+
+
@@ Tokens @@
List[106]:
  int
  identifier(main)
  (
  void
  )
  {
  long
  identifier(arr)
  [
  const int(4)
  ]
  =
  {
  -
  const int(1)
  ,
  -
  const int(2)
  ,
  -
  const int(3)
  ,
  -
  const int(4)
  }
  ;
  if
  (
  identifier(arr)
  !=
  (
  long
  *
  )
  identifier(arr)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  (
  long
  (
  *
  )
  [
  const int(4)
  ]
  )
  identifier(arr)
  !=
  &
  identifier(arr)
  )
  {
  return
  const int(2)
  ;
  }
  unsigned
  long
  *
  identifier(unsigned_arr)
  =
  (
  unsigned
  long
  *
  )
  identifier(arr)
  ;
  if
  (
  identifier(unsigned_arr)
  [
  const int(0)
  ]
  !=
  const unsigned long(18446744073709551615UL)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(unsigned_arr)
  [
  const int(3)
  ]
  !=
  const unsigned long(18446744073709551612UL)
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
