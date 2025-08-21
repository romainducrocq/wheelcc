-- Lexing ... OK
+
+
@@ Tokens @@
List[136]:
  void
  *
  identifier(get_100_zeroed_bytes)
  (
  void
  )
  ;
  void
  identifier(fill_100_bytes)
  (
  void
  *
  identifier(pointer)
  ,
  int
  identifier(byte)
  )
  ;
  void
  identifier(free_bytes)
  (
  void
  *
  identifier(ptr)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  void
  *
  identifier(mem)
  =
  identifier(get_100_zeroed_bytes)
  (
  )
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(100)
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
  (
  (
  char
  *
  )
  identifier(mem)
  +
  identifier(i)
  )
  [
  const int(0)
  ]
  )
  {
  return
  const int(1)
  ;
  }
  }
  identifier(fill_100_bytes)
  (
  identifier(mem)
  ,
  const int(99)
  )
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(100)
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
  (
  (
  char
  *
  )
  identifier(mem)
  +
  identifier(i)
  )
  [
  const int(0)
  ]
  !=
  const int(99)
  )
  {
  return
  const int(2)
  ;
  }
  }
  identifier(free_bytes)
  (
  identifier(mem)
  )
  ;
  return
  const int(0)
  ;
  }
