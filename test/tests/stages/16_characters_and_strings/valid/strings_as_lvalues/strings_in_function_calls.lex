-- Lexing ... OK
+
+
@@ Tokens @@
List[198]:
  unsigned
  long
  identifier(strlen)
  (
  char
  *
  identifier(s)
  )
  ;
  char
  *
  identifier(return_string)
  (
  void
  )
  {
  return
  string literal("I'm a string!")
  ;
  }
  int
  identifier(pass_string_args)
  (
  char
  *
  identifier(s1)
  ,
  char
  *
  identifier(s2)
  )
  {
  if
  (
  identifier(s1)
  ==
  const int(0)
  ||
  identifier(s2)
  ==
  const int(0)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(strlen)
  (
  identifier(s1)
  )
  !=
  const int(45)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(s1)
  [
  const int(41)
  ]
  !=
  const char('d')
  ||
  identifier(s1)
  [
  const int(42)
  ]
  !=
  const char('o')
  ||
  identifier(s1)
  [
  const int(43)
  ]
  !=
  const char('g')
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(s2)
  [
  const int(0)
  ]
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  char
  *
  identifier(ptr)
  =
  const int(0)
  ;
  identifier(ptr)
  =
  identifier(return_string)
  (
  )
  ;
  if
  (
  !
  identifier(ptr)
  )
  return
  const int(1)
  ;
  if
  (
  identifier(ptr)
  [
  const int(0)
  ]
  !=
  const char('I')
  ||
  identifier(ptr)
  [
  const int(1)
  ]
  !=
  const char('\'')
  ||
  identifier(ptr)
  [
  const int(13)
  ]
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  !
  identifier(pass_string_args)
  (
  string literal("The quick brown fox jumped over the lazy dog.")
  ,
  string literal("")
  )
  )
  {
  return
  const int(3)
  ;
  }
  return
  const int(0)
  ;
  char
  *
  identifier(ptr2)
  ;
  identifier(ptr2)
  =
  const int(1)
  ?
  identifier(ptr)
  +
  const int(2)
  :
  identifier(ptr)
  +
  const int(4)
  ;
  return
  *
  identifier(ptr2)
  ==
  const char('m')
  ;
  }
