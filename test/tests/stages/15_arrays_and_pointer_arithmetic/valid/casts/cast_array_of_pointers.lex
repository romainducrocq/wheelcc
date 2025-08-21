-- Lexing ... OK
+
+
@@ Tokens @@
List[66]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(simple_array)
  [
  const int(2)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  }
  ;
  int
  (
  *
  identifier(ptr_arr)
  [
  const int(3)
  ]
  )
  [
  const int(2)
  ]
  =
  {
  &
  identifier(simple_array)
  ,
  const int(0)
  ,
  &
  identifier(simple_array)
  }
  ;
  long
  *
  identifier(other_ptr)
  =
  (
  long
  *
  )
  identifier(ptr_arr)
  ;
  return
  (
  int
  (
  *
  *
  )
  [
  const int(2)
  ]
  )
  identifier(other_ptr)
  ==
  identifier(ptr_arr)
  ;
  }
