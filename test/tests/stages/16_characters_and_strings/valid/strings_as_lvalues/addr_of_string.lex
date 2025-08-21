-- Lexing ... OK
+
+
@@ Tokens @@
List[73]:
  int
  identifier(puts)
  (
  char
  *
  identifier(s)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  char
  (
  *
  identifier(str)
  )
  [
  const int(16)
  ]
  =
  &
  string literal("Sample\tstring!\n")
  ;
  identifier(puts)
  (
  *
  identifier(str)
  )
  ;
  char
  (
  *
  identifier(one_past_the_end)
  )
  [
  const int(16)
  ]
  =
  identifier(str)
  +
  const int(1)
  ;
  char
  *
  identifier(last_byte_pointer)
  =
  (
  char
  *
  )
  identifier(one_past_the_end)
  -
  const int(1)
  ;
  if
  (
  *
  identifier(last_byte_pointer)
  !=
  const int(0)
  )
  {
  return
  const int(1)
  ;
  }
  return
  const int(0)
  ;
  }
