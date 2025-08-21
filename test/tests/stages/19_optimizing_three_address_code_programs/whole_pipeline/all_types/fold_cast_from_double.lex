-- Lexing ... OK
+
+
@@ Tokens @@
List[94]:
  char
  identifier(target_to_char)
  (
  void
  )
  {
  return
  (
  char
  )
  -
  const double(126.5)
  ;
  }
  int
  identifier(target_to_int)
  (
  void
  )
  {
  return
  (
  int
  )
  -
  const double(5.9)
  ;
  }
  long
  identifier(target_to_long)
  (
  void
  )
  {
  return
  (
  long
  )
  -
  const double(9223372036854774783.1)
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
  identifier(target_to_char)
  (
  )
  !=
  -
  const int(126)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(target_to_int)
  (
  )
  !=
  -
  const int(5)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_to_long)
  (
  )
  !=
  -
  const long(9223372036854774784l)
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
