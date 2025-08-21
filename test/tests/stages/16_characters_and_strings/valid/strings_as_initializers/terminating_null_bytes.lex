-- Lexing ... OK
+
+
@@ Tokens @@
List[767]:
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
  identifier(test_flat_static_with_null_byte)
  (
  void
  )
  {
  static
  unsigned
  char
  identifier(flat)
  [
  const int(4)
  ]
  =
  string literal("dog")
  ;
  return
  (
  identifier(flat)
  [
  const int(0)
  ]
  ==
  const char('d')
  &&
  identifier(flat)
  [
  const int(1)
  ]
  ==
  const char('o')
  &&
  identifier(flat)
  [
  const int(2)
  ]
  ==
  const char('g')
  &&
  identifier(flat)
  [
  const int(3)
  ]
  ==
  const int(0)
  )
  ;
  }
  int
  identifier(test_nested_static_with_null_byte)
  (
  void
  )
  {
  static
  char
  identifier(nested)
  [
  const int(2)
  ]
  [
  const int(4)
  ]
  =
  {
  string literal("yes")
  ,
  string literal("yup")
  }
  ;
  return
  (
  identifier(nested)
  [
  const int(0)
  ]
  [
  const int(0)
  ]
  ==
  const char('y')
  &&
  identifier(nested)
  [
  const int(0)
  ]
  [
  const int(1)
  ]
  ==
  const char('e')
  &&
  identifier(nested)
  [
  const int(0)
  ]
  [
  const int(2)
  ]
  ==
  const char('s')
  &&
  identifier(nested)
  [
  const int(0)
  ]
  [
  const int(3)
  ]
  ==
  const int(0)
  &&
  identifier(nested)
  [
  const int(1)
  ]
  [
  const int(0)
  ]
  ==
  const char('y')
  &&
  identifier(nested)
  [
  const int(1)
  ]
  [
  const int(1)
  ]
  ==
  const char('u')
  &&
  identifier(nested)
  [
  const int(1)
  ]
  [
  const int(2)
  ]
  ==
  const char('p')
  &&
  identifier(nested)
  [
  const int(1)
  ]
  [
  const int(3)
  ]
  ==
  const int(0)
  )
  ;
  }
  int
  identifier(test_flat_auto_with_null_byte)
  (
  void
  )
  {
  char
  identifier(flat_auto)
  [
  const int(2)
  ]
  =
  string literal("x")
  ;
  return
  (
  identifier(flat_auto)
  [
  const int(0)
  ]
  ==
  const char('x')
  &&
  identifier(flat_auto)
  [
  const int(1)
  ]
  ==
  const int(0)
  )
  ;
  }
  int
  identifier(test_nested_auto_with_null_byte)
  (
  void
  )
  {
  char
  identifier(nested_auto)
  [
  const int(2)
  ]
  [
  const int(2)
  ]
  [
  const int(2)
  ]
  =
  {
  {
  string literal("a")
  ,
  string literal("b")
  }
  ,
  {
  string literal("c")
  ,
  string literal("d")
  }
  }
  ;
  return
  (
  identifier(nested_auto)
  [
  const int(0)
  ]
  [
  const int(0)
  ]
  [
  const int(0)
  ]
  ==
  const char('a')
  &&
  identifier(nested_auto)
  [
  const int(0)
  ]
  [
  const int(0)
  ]
  [
  const int(1)
  ]
  ==
  const int(0)
  &&
  identifier(nested_auto)
  [
  const int(0)
  ]
  [
  const int(1)
  ]
  [
  const int(0)
  ]
  ==
  const char('b')
  &&
  identifier(nested_auto)
  [
  const int(0)
  ]
  [
  const int(1)
  ]
  [
  const int(1)
  ]
  ==
  const int(0)
  &&
  identifier(nested_auto)
  [
  const int(1)
  ]
  [
  const int(0)
  ]
  [
  const int(0)
  ]
  ==
  const char('c')
  &&
  identifier(nested_auto)
  [
  const int(1)
  ]
  [
  const int(0)
  ]
  [
  const int(1)
  ]
  ==
  const int(0)
  &&
  identifier(nested_auto)
  [
  const int(1)
  ]
  [
  const int(1)
  ]
  [
  const int(0)
  ]
  ==
  const char('d')
  &&
  identifier(nested_auto)
  [
  const int(1)
  ]
  [
  const int(1)
  ]
  [
  const int(1)
  ]
  ==
  const int(0)
  )
  ;
  }
  int
  identifier(test_flat_static_without_null_byte)
  (
  void
  )
  {
  static
  char
  identifier(letters)
  [
  const int(4)
  ]
  =
  string literal("abcd")
  ;
  return
  identifier(letters)
  [
  const int(0)
  ]
  ==
  const char('a')
  &&
  identifier(letters)
  [
  const int(1)
  ]
  ==
  const char('b')
  &&
  identifier(letters)
  [
  const int(2)
  ]
  ==
  const char('c')
  &&
  identifier(letters)
  [
  const int(3)
  ]
  ==
  const char('d')
  ;
  }
  char
  identifier(nested)
  [
  const int(3)
  ]
  [
  const int(3)
  ]
  =
  {
  string literal("yes")
  ,
  string literal("no")
  ,
  string literal("ok")
  }
  ;
  int
  identifier(test_nested_static_without_null_byte)
  (
  void
  )
  {
  char
  *
  identifier(whole_array)
  =
  (
  char
  *
  )
  identifier(nested)
  ;
  char
  *
  identifier(word1)
  =
  (
  char
  *
  )
  identifier(nested)
  [
  const int(0)
  ]
  ;
  char
  *
  identifier(word2)
  =
  (
  char
  *
  )
  identifier(nested)
  [
  const int(1)
  ]
  ;
  char
  *
  identifier(word3)
  =
  (
  char
  *
  )
  identifier(nested)
  [
  const int(2)
  ]
  ;
  return
  !
  (
  identifier(strcmp)
  (
  identifier(whole_array)
  ,
  string literal("yesno")
  )
  ||
  identifier(strcmp)
  (
  identifier(word1)
  ,
  string literal("yesno")
  )
  ||
  identifier(strcmp)
  (
  identifier(word2)
  ,
  string literal("no")
  )
  ||
  identifier(strcmp)
  (
  identifier(word3)
  ,
  string literal("ok")
  )
  )
  ;
  }
  int
  identifier(test_flat_auto_without_null_byte)
  (
  void
  )
  {
  int
  identifier(x)
  =
  -
  const int(1)
  ;
  char
  identifier(letters)
  [
  const int(4)
  ]
  =
  string literal("abcd")
  ;
  int
  identifier(y)
  =
  -
  const int(1)
  ;
  return
  (
  identifier(x)
  ==
  -
  const int(1)
  &&
  identifier(y)
  ==
  -
  const int(1)
  &&
  identifier(letters)
  [
  const int(0)
  ]
  ==
  const char('a')
  &&
  identifier(letters)
  [
  const int(1)
  ]
  ==
  const char('b')
  &&
  identifier(letters)
  [
  const int(2)
  ]
  ==
  const char('c')
  &&
  identifier(letters)
  [
  const int(3)
  ]
  ==
  const char('d')
  )
  ;
  }
  int
  identifier(test_nested_auto_without_null_byte)
  (
  void
  )
  {
  char
  identifier(nested)
  [
  const int(3)
  ]
  [
  const int(3)
  ]
  =
  {
  string literal("yes")
  ,
  string literal("no")
  ,
  string literal("ok")
  }
  ;
  char
  *
  identifier(whole_array)
  =
  (
  char
  *
  )
  identifier(nested)
  ;
  char
  *
  identifier(word1)
  =
  (
  char
  *
  )
  identifier(nested)
  [
  const int(0)
  ]
  ;
  char
  *
  identifier(word2)
  =
  (
  char
  *
  )
  identifier(nested)
  [
  const int(1)
  ]
  ;
  char
  *
  identifier(word3)
  =
  (
  char
  *
  )
  identifier(nested)
  [
  const int(2)
  ]
  ;
  return
  !
  (
  identifier(strcmp)
  (
  identifier(whole_array)
  ,
  string literal("yesno")
  )
  ||
  identifier(strcmp)
  (
  identifier(word1)
  ,
  string literal("yesno")
  )
  ||
  identifier(strcmp)
  (
  identifier(word2)
  ,
  string literal("no")
  )
  ||
  identifier(strcmp)
  (
  identifier(word3)
  ,
  string literal("ok")
  )
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
  identifier(test_flat_static_with_null_byte)
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
  identifier(test_nested_static_with_null_byte)
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
  identifier(test_flat_auto_with_null_byte)
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
  identifier(test_nested_auto_with_null_byte)
  (
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
  identifier(test_flat_static_without_null_byte)
  (
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
  identifier(test_nested_static_without_null_byte)
  (
  )
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  !
  identifier(test_flat_auto_without_null_byte)
  (
  )
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  !
  identifier(test_nested_auto_without_null_byte)
  (
  )
  )
  {
  return
  const int(8)
  ;
  }
  return
  const int(0)
  ;
  }
