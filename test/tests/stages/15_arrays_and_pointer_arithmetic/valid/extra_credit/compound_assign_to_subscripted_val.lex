-- Lexing ... OK
+
+
@@ Tokens @@
List[210]:
  unsigned
  identifier(unsigned_arr)
  [
  const int(4)
  ]
  =
  {
  const unsigned int(4294967295U)
  ,
  const unsigned int(4294967294U)
  ,
  const unsigned int(4294967293U)
  ,
  const unsigned int(4294967292U)
  }
  ;
  int
  identifier(idx)
  =
  const int(2)
  ;
  long
  identifier(long_idx)
  =
  const int(1)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  identifier(long_idx)
  =
  -
  identifier(long_idx)
  ;
  identifier(unsigned_arr)
  [
  const int(1)
  ]
  +=
  const int(2)
  ;
  if
  (
  identifier(unsigned_arr)
  [
  const int(1)
  ]
  )
  {
  return
  const int(1)
  ;
  }
  identifier(unsigned_arr)
  [
  identifier(idx)
  ]
  -=
  const double(10.0)
  ;
  if
  (
  identifier(unsigned_arr)
  [
  identifier(idx)
  ]
  !=
  const unsigned int(4294967283U)
  )
  {
  return
  const int(2)
  ;
  }
  unsigned
  *
  identifier(unsigned_ptr)
  =
  identifier(unsigned_arr)
  +
  const int(4)
  ;
  identifier(unsigned_ptr)
  [
  identifier(long_idx)
  ]
  /=
  const int(10)
  ;
  if
  (
  identifier(unsigned_arr)
  [
  const int(3)
  ]
  !=
  const unsigned int(429496729U)
  )
  {
  return
  const int(3)
  ;
  }
  identifier(unsigned_ptr)
  [
  identifier(long_idx)
  *=
  const int(2)
  ]
  *=
  identifier(unsigned_arr)
  [
  const int(0)
  ]
  ;
  if
  (
  identifier(unsigned_arr)
  [
  const int(2)
  ]
  !=
  const int(13)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  (
  identifier(unsigned_arr)
  [
  identifier(idx)
  +
  identifier(long_idx)
  ]
  %=
  const int(10)
  )
  !=
  const int(5)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(unsigned_arr)
  [
  const int(0)
  ]
  !=
  const unsigned int(5u)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(unsigned_arr)
  [
  const int(1)
  ]
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(unsigned_arr)
  [
  const int(2)
  ]
  !=
  const int(13)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(unsigned_arr)
  [
  const int(3)
  ]
  !=
  const unsigned int(429496729U)
  )
  {
  return
  const int(9)
  ;
  }
  return
  const int(0)
  ;
  }
