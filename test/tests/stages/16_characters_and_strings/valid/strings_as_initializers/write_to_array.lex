-- Lexing ... OK
+
+
@@ Tokens @@
List[92]:
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
  identifier(flat_arr)
  [
  const int(4)
  ]
  =
  string literal("abc")
  ;
  identifier(puts)
  (
  identifier(flat_arr)
  )
  ;
  identifier(flat_arr)
  [
  const int(2)
  ]
  =
  const char('x')
  ;
  identifier(puts)
  (
  identifier(flat_arr)
  )
  ;
  char
  identifier(nested_array)
  [
  const int(2)
  ]
  [
  const int(6)
  ]
  =
  {
  string literal("Hello")
  ,
  string literal("World")
  }
  ;
  identifier(puts)
  (
  identifier(nested_array)
  [
  const int(0)
  ]
  )
  ;
  identifier(puts)
  (
  identifier(nested_array)
  [
  const int(1)
  ]
  )
  ;
  identifier(nested_array)
  [
  const int(0)
  ]
  [
  const int(0)
  ]
  =
  const char('J')
  ;
  identifier(puts)
  (
  identifier(nested_array)
  [
  const int(0)
  ]
  )
  ;
  return
  const int(0)
  ;
  }
