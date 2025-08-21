-- Lexing ... OK
+
+
@@ Tokens @@
List[181]:
  int
  identifier(puts)
  (
  char
  *
  identifier(s)
  )
  ;
  int
  identifier(strcmp)
  (
  char
  *
  identifier(s1)
  ,
  char
  *
  identifier(s2)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  char
  *
  identifier(escape_sequence)
  =
  string literal("\a\b")
  ;
  if
  (
  identifier(escape_sequence)
  [
  const int(0)
  ]
  !=
  const int(7)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(escape_sequence)
  [
  const int(1)
  ]
  !=
  const int(8)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(escape_sequence)
  [
  const int(2)
  ]
  )
  {
  return
  const int(3)
  ;
  }
  char
  *
  identifier(with_double_quote)
  =
  string literal("Hello\"world")
  ;
  if
  (
  identifier(with_double_quote)
  [
  const int(5)
  ]
  !=
  const char('"')
  )
  {
  return
  const int(4)
  ;
  }
  identifier(puts)
  (
  identifier(with_double_quote)
  )
  ;
  char
  *
  identifier(with_backslash)
  =
  string literal("Hello\\World")
  ;
  if
  (
  identifier(with_backslash)
  [
  const int(5)
  ]
  !=
  const char('\\')
  )
  {
  return
  const int(5)
  ;
  }
  identifier(puts)
  (
  identifier(with_backslash)
  )
  ;
  char
  *
  identifier(with_newline)
  =
  string literal("Line\nbreak!")
  ;
  if
  (
  identifier(with_newline)
  [
  const int(4)
  ]
  !=
  const int(10)
  )
  {
  return
  const int(6)
  ;
  }
  identifier(puts)
  (
  identifier(with_newline)
  )
  ;
  char
  *
  identifier(tab)
  =
  string literal("	")
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(tab)
  ,
  string literal("\t")
  )
  )
  {
  return
  const int(7)
  ;
  }
  identifier(puts)
  (
  string literal("Testing, 123.")
  )
  ;
  identifier(puts)
  (
  string literal("^@1 _\\]")
  )
  ;
  return
  const int(0)
  ;
  }
