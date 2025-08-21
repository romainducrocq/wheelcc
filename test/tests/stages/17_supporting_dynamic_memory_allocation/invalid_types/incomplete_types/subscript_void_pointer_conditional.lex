-- Lexing ... OK
+
+
@@ Tokens @@
List[47]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(arr)
  [
  const int(3)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  }
  ;
  void
  *
  identifier(void_ptr)
  =
  identifier(arr)
  ;
  int
  *
  identifier(int_ptr)
  =
  identifier(arr)
  +
  const int(1)
  ;
  return
  (
  const int(1)
  ?
  identifier(int_ptr)
  :
  identifier(void_ptr)
  )
  [
  const int(1)
  ]
  ;
  }
