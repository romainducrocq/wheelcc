-- Lexing ... OK
+
+
@@ Tokens @@
List[68]:
  struct
  identifier(chars)
  {
  char
  identifier(char_array)
  [
  const int(5)
  ]
  ;
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(chars)
  identifier(x)
  =
  {
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  ,
  const int(4)
  ,
  const int(5)
  }
  }
  ;
  char
  identifier(arr)
  [
  const int(5)
  ]
  =
  {
  const int(9)
  ,
  const int(8)
  ,
  const int(7)
  ,
  const int(6)
  ,
  const int(5)
  }
  ;
  identifier(x)
  .
  identifier(char_array)
  =
  identifier(arr)
  ;
  return
  identifier(x)
  .
  identifier(char_array)
  [
  const int(0)
  ]
  ;
  }
