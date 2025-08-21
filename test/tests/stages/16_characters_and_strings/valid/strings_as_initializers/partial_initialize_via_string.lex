-- Lexing ... OK
+
+
@@ Tokens @@
List[508]:
  static
  char
  identifier(static_arr)
  [
  const int(5)
  ]
  =
  string literal("hi")
  ;
  int
  identifier(test_static)
  (
  void
  )
  {
  return
  (
  identifier(static_arr)
  [
  const int(0)
  ]
  ==
  const char('h')
  &&
  identifier(static_arr)
  [
  const int(1)
  ]
  ==
  const char('i')
  &&
  !
  (
  identifier(static_arr)
  [
  const int(2)
  ]
  ||
  identifier(static_arr)
  [
  const int(3)
  ]
  ||
  identifier(static_arr)
  [
  const int(4)
  ]
  )
  )
  ;
  }
  static
  signed
  char
  identifier(nested_static_arr)
  [
  const int(3)
  ]
  [
  const int(4)
  ]
  =
  {
  string literal("")
  ,
  string literal("bc")
  }
  ;
  int
  identifier(test_static_nested)
  (
  void
  )
  {
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(3)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  for
  (
  int
  identifier(j)
  =
  const int(0)
  ;
  identifier(j)
  <
  const int(4)
  ;
  identifier(j)
  =
  identifier(j)
  +
  const int(1)
  )
  {
  signed
  char
  identifier(c)
  =
  identifier(nested_static_arr)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  ;
  signed
  char
  identifier(expected)
  =
  const int(0)
  ;
  if
  (
  identifier(i)
  ==
  const int(1)
  &&
  identifier(j)
  ==
  const int(0)
  )
  {
  identifier(expected)
  =
  const char('b')
  ;
  }
  else
  if
  (
  identifier(i)
  ==
  const int(1)
  &&
  identifier(j)
  ==
  const int(1)
  )
  {
  identifier(expected)
  =
  const char('c')
  ;
  }
  if
  (
  identifier(c)
  !=
  identifier(expected)
  )
  {
  return
  const int(0)
  ;
  }
  }
  return
  const int(1)
  ;
  }
  int
  identifier(test_automatic)
  (
  void
  )
  {
  unsigned
  char
  identifier(aut)
  [
  const int(4)
  ]
  =
  string literal("ab")
  ;
  return
  (
  identifier(aut)
  [
  const int(0)
  ]
  ==
  const char('a')
  &&
  identifier(aut)
  [
  const int(1)
  ]
  ==
  const char('b')
  &&
  !
  (
  identifier(aut)
  [
  const int(2)
  ]
  ||
  identifier(aut)
  [
  const int(3)
  ]
  )
  )
  ;
  }
  int
  identifier(test_automatic_nested)
  (
  void
  )
  {
  signed
  char
  identifier(nested_auto)
  [
  const int(2)
  ]
  [
  const int(2)
  ]
  [
  const int(4)
  ]
  =
  {
  {
  string literal("foo")
  }
  ,
  {
  string literal("x")
  ,
  string literal("yz")
  }
  }
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
  const int(2)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  for
  (
  int
  identifier(j)
  =
  const int(0)
  ;
  identifier(j)
  <
  const int(2)
  ;
  identifier(j)
  =
  identifier(j)
  +
  const int(1)
  )
  {
  for
  (
  int
  identifier(k)
  =
  const int(0)
  ;
  identifier(k)
  <
  const int(4)
  ;
  identifier(k)
  =
  identifier(k)
  +
  const int(1)
  )
  {
  signed
  char
  identifier(c)
  =
  identifier(nested_auto)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  [
  identifier(k)
  ]
  ;
  signed
  char
  identifier(expected)
  =
  const int(0)
  ;
  if
  (
  identifier(i)
  ==
  const int(0)
  &&
  identifier(j)
  ==
  const int(0)
  )
  {
  if
  (
  identifier(k)
  ==
  const int(0)
  )
  {
  identifier(expected)
  =
  const char('f')
  ;
  }
  else
  if
  (
  identifier(k)
  ==
  const int(1)
  ||
  identifier(k)
  ==
  const int(2)
  )
  {
  identifier(expected)
  =
  const char('o')
  ;
  }
  }
  else
  if
  (
  identifier(i)
  ==
  const int(1)
  &&
  identifier(j)
  ==
  const int(0)
  &&
  identifier(k)
  ==
  const int(0)
  )
  {
  identifier(expected)
  =
  const char('x')
  ;
  }
  else
  if
  (
  identifier(i)
  ==
  const int(1)
  &&
  identifier(j)
  ==
  const int(1)
  &&
  identifier(k)
  ==
  const int(0)
  )
  {
  identifier(expected)
  =
  const char('y')
  ;
  }
  else
  if
  (
  identifier(i)
  ==
  const int(1)
  &&
  identifier(j)
  ==
  const int(1)
  &&
  identifier(k)
  ==
  const int(1)
  )
  {
  identifier(expected)
  =
  const char('z')
  ;
  }
  if
  (
  identifier(c)
  !=
  identifier(expected)
  )
  {
  return
  const int(0)
  ;
  }
  }
  }
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
  if
  (
  !
  identifier(test_static)
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
  identifier(test_static_nested)
  (
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
  identifier(test_automatic)
  (
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
  identifier(test_automatic_nested)
  (
  )
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
