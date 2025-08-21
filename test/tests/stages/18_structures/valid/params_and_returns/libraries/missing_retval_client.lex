-- Lexing ... OK
+
+
@@ Tokens @@
List[58]:
  struct
  identifier(big)
  {
  char
  identifier(arr)
  [
  const int(25)
  ]
  ;
  }
  ;
  struct
  identifier(big)
  identifier(missing_return_value)
  (
  int
  *
  identifier(i)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(array)
  [
  const int(4)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  ,
  const int(4)
  }
  ;
  identifier(missing_return_value)
  (
  identifier(array)
  +
  const int(2)
  )
  ;
  return
  identifier(array)
  [
  const int(2)
  ]
  ==
  const int(10)
  ;
  }
